#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include "PrimitiveDrawer.h"

GameScene::GameScene() {}

GameScene::~GameScene() {
	delete debugCamera_;
	delete model_;
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	debugText_ = DebugText::GetInstance();
	//デバックカメラの生成
	debugCamera_ = new DebugCamera(600, 400);

	//ここをいじる
	PrimitiveDrawer::GetInstance()->SetViewProjection(&debugCamera_->GetViewProjection());

	//軸方向表示を有効にする
	AxisIndicator::GetInstance()->SetVisible(true);
	//軸方向表示が参照するビュープロジェクションを指定する（アドレス渡し）
	AxisIndicator::GetInstance()->SetTargetViewProjection(&debugCamera_->GetViewProjection());

	//ベクトルの計算
	vv1 = { vertex_[1].x - vertex_[0].x,vertex_[1].y - vertex_[0].y,vertex_[1].z - vertex_[0].z };//ベクトル１つ目
	vv2 = { vertex_[2].x - vertex_[1].x,vertex_[2].y - vertex_[1].y,vertex_[2].z - vertex_[1].z };//ベクトル２つ目
	//外積の計算
	Vec3Cross(&n1, &vv1, &vv2);
	//正規化
	Vec3Normalize(&n1, &n1);

	//ワールドトランスフォームの初期化
	for (int i = 0; i < 2; i++) {
		worldTransform_[i].Initialize();
	}
	viewTransForm.Initialize();

	worldTransform_[0].translation_ = { 10,0,0 };

	worldTransform_[1].translation_ = { 0,0,0 };
	for (int i = 0; i < 2; i++) {
		Afin(worldTransform_[i], viewContainer);
	}
	worldTransform_[0].TransferMatrix();
	worldTransform_[1].TransferMatrix();
	//ビュープロジェクションの初期化
	viewProjection_.Initialize();


	model_ = Model::CreateFromOBJ("cube", true);
	pModel_ = Model::CreateFromOBJ("cube", true);

	textureHandle_ = TextureManager::Load("mario.jpg");


}

void GameScene::Update() {
	debugCamera_->Update();
	//デバックカメラの座標をとる変数
	eyes = { debugCamera_->GetViewProjection().eye.x,debugCamera_->GetViewProjection().eye.y,debugCamera_->GetViewProjection().eye.z };
	trans = { debugCamera_->GetViewProjection().target.x,debugCamera_->GetViewProjection().target.y,debugCamera_->GetViewProjection().target.z  };
	/*up = { 0.0f,1.0f,0.0f };*/
	viewP = { debugCamera_->GetViewProjection().up.x,debugCamera_->GetViewProjection().up.y,debugCamera_->GetViewProjection().up.z };
	//カメラと法線の内積
	intterProdict = Vec3Dot(&n1, &eyes);

	//カメラと逆ベクトルを計算
	//MatrixIdentity(viewContainer2);
	//MatrixInverse(inverseMatrix, &viewProjection_.matView);

	float speedX = 0;
	float speedY = 0;
	float speedZ = 0;
	if (DIK_A == 1) {
		speedX -= 1;
	}
	if (DIK_D == 1) {
		speedX += 1;
	}
	if (DIK_S == 1) {
		speedZ += 1;
	}
	if (DIK_W == 1) {
		speedZ -= 1;
	}
	if (DIK_UP == 1) {
		speedZ -= 1;
	}
	if (DIK_DOWN == 1) {
		speedZ += 1;
	}


	worldTransform_[1].translation_.x += speedX;
	worldTransform_[1].translation_.y += speedY;
	worldTransform_[1].translation_.z += speedZ;

	MatrixIdentity(viewContainer);
	MatrixIdentity(viewContainer2);
	viewContainer *= debugCamera_->GetViewProjection().matView;
	//ビルボードを求める関数
	GetInvRotateMat(&worldTransform_[1].translation_, &eyes, viewContainer2, viewP);

	MatrixInverse(viewContainer, viewContainer);

	/*MakeInverse(viewContainer2, viewContainer);*/
	/*viewContainer2 *= viewContainer;*/
	MatrixIdentity(worldTransform_[1].matWorld_);

	AfinMatRot(worldTransform_[1], viewContainer2);
	//AfinMatRot(worldTransform_[1], viewP);
	AfinTransform(worldTransform_[1]);
	worldTransform_[1].TransferMatrix();

	//内積の値
	debugText_->SetPos(50, 50);
	debugText_->Printf(
		"intterProduct:(%f)", intterProdict);
	//視点の座標
	debugText_->SetPos(50, 70);
	debugText_->Printf(
		"debugCameraPosition:(x %f\n,y %f\n,z %f\n)", eyes.x, eyes.y, eyes.z);
	////視点の座標
	//debugText_->SetPos(50, 190);
	//debugText_->Printf(
	//	"debugCameraPosition:(x %f\n,y %f\n,z %f\n)", trans.x, trans.y, trans.z);
	////視点の座標
	//debugText_->SetPos(50, 170);
	//debugText_->Printf(
	//	"debugCameraPosition:(x %f\n,y %f\n,z %f\n)", up.x, up.y, up.z);
	////表か裏か
	if (intterProdict > 0) {
		debugText_->SetPos(50, 90);
		debugText_->Printf(
			"Head");
	}
	else
	{
		debugText_->SetPos(50, 90);
		debugText_->Printf(
			"tail");
	}
	debugText_->SetPos(50, 120);
	debugText_->Printf(
		"%f	%f	%f", eyes.x, eyes.y, eyes.z);

	debugText_->SetPos(50, 140);
	debugText_->Printf(
		"x  %f	y  %f	z  %f", worldTransform_[1].matWorld_.m[0][0], worldTransform_[1].matWorld_.m[0][1], worldTransform_[1].matWorld_.m[0][2]);
	debugText_->SetPos(50, 160);
	debugText_->Printf(
		"x  %f	y  %f	z  %f", worldTransform_[1].matWorld_.m[1][0], worldTransform_[1].matWorld_.m[1][1], worldTransform_[1].matWorld_.m[1][2]);
	debugText_->SetPos(50, 180);
	debugText_->Printf(
		"x  %f	y  %f	z  %f", worldTransform_[1].matWorld_.m[2][0], worldTransform_[1].matWorld_.m[2][1], worldTransform_[1].matWorld_.m[2][2]);
	debugText_->SetPos(50, 200);
	debugText_->Printf(
		"x  %f	y  %f	z  %f", worldTransform_[1].matWorld_.m[3][0], worldTransform_[1].matWorld_.m[3][1], worldTransform_[1].matWorld_.m[3][2]);

	debugText_->SetPos(50,240);
	debugText_->Printf(
		"x  %f	y  %f	z  %f", viewContainer.m[0][0], viewContainer.m[0][1], viewContainer.m[0][2]);
	debugText_->SetPos(50,260);
	debugText_->Printf(
		"x  %f	y  %f	z  %f", viewContainer.m[1][0], viewContainer.m[1][1], viewContainer.m[1][2]);
	debugText_->SetPos(50, 280);
	debugText_->Printf(
		"x  %f	y  %f	z  %f", viewContainer.m[2][0], viewContainer.m[2][1], viewContainer.m[2][2]);
	debugText_->SetPos(50, 300);
	debugText_->Printf(
		"x  %f	y  %f	z  %f", viewContainer.m[3][0], viewContainer.m[3][1], viewContainer.m[3][2]);

}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	//model_->Draw(worldTransform_[0], viewProjection_, textureHandle_);
	//pModel_->Draw(worldTransform_[1], viewProjection_, textureHandle_);
	//ライン描画が参照するビュープロジェクションを指定する（アドレス渡し）

	model_->Draw(worldTransform_[0], debugCamera_->GetViewProjection(), textureHandle_);
	pModel_->Draw(worldTransform_[1], debugCamera_->GetViewProjection(), textureHandle_);

	for (int i = 0; i < 4; i++) {
		PrimitiveDrawer::GetInstance()->DrawLine3d(vertex_[eageList_[i][0]], vertex_[eageList_[i][1]], Vector4(0xff, 0xff, 0xff, 0xff));
	}
	PrimitiveDrawer::GetInstance()->DrawLine3d(Vector3(0, 0, 0), n1, Vector4(0xff, 0xff, 0xff, 0xff));
	/*model_->Draw(worldTransform_[0], debugCamera_->GetViewProjection());
	pModel_->Draw(worldTransform_[1], debugCamera_->GetViewProjection());*/
	/*if (viewSelct == 1) {
		model_->Draw(worldTransform_[0], viewProjection_);
		if (intterProdict >= 0) {
			pModel_->Draw(worldTransform_[1], viewProjection_);
		}
	}*/
	// 3Dオブジェクト描画後処理
	Model::PostDraw();


#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// デバッグテキストの描画
	debugText_->DrawAll(commandList);
	//
	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}

int GameScene::Vec3Normalize(Vector3* pOut, Vector3* pV)
{
	double len;
	double x, y, z;

	x = (double)(pV->x);
	y = (double)(pV->y);
	z = (double)(pV->z);
	len = sqrt(x * x + y * y + z * z);

	if (len < (1e-6)) return 0;

	len = 1.0 / len;
	x *= len;
	y *= len;
	z *= len;

	pOut->x = (float)x;
	pOut->y = (float)y;
	pOut->z = (float)z;

	return 1;
}

float GameScene::Vec3Dot(Vector3* pV1, Vector3* pV2)
{
	return ((pV1->x) * (pV2->x) + (pV1->y) * (pV2->y) + (pV1->z) * (pV2->z));
}

void GameScene::Vec3Cross(Vector3* pOut, Vector3* pV1, Vector3* pV2)
{
	Vector3 vec;
	double x1, y1, z1, x2, y2, z2;

	x1 = (double)(pV1->x);
	y1 = (double)(pV1->y);
	z1 = (double)(pV1->z);
	x2 = (double)(pV2->x);
	y2 = (double)(pV2->y);
	z2 = (double)(pV2->z);

	vec.x = (float)(y1 * z2 - z1 * y2);
	vec.y = (float)(z1 * x2 - x1 * z2);
	vec.z = (float)(x1 * y2 - y1 * x2);
	*pOut = vec;
}

void GameScene::Afin(WorldTransform& worldTransform_, Matrix4& viewWorldMatRot)
{
	Matrix4 matScale;
	Matrix4 matRot;
	Matrix4 matRotX;
	Matrix4 matRotY;
	Matrix4 matRotZ;
	Matrix4 matTrans;
	matScale = {
	worldTransform_.scale_.x,0,0,0,
	0,worldTransform_.scale_.y,0,0,
	0,0,worldTransform_.scale_.z,0,
	0,0,0,1
	};
	matTrans = {
	1,0,0,0,
	0,1,0,0,
	0,0,1,0,
	worldTransform_.translation_.x,worldTransform_.translation_.y,worldTransform_.translation_.z,1
	};
	//Z用回転軸の設定
	matRotZ = {
	cos(worldTransform_.rotation_.z),sin(worldTransform_.rotation_.z),0,0,
	-sin(worldTransform_.rotation_.z),cos(worldTransform_.rotation_.z),0,0,
	0,0,1,0,
	0,0,0,1
	};

	//X用回転軸の設定
	matRotX = {
	1,0,0,0,
	0,cos(worldTransform_.rotation_.x),sin(worldTransform_.rotation_.x),0,
	0,-sin(worldTransform_.rotation_.x),cos(worldTransform_.rotation_.x),0,
	0,0,0,1
	};

	//Y用回転軸の設定
	matRotY = {
	cos(worldTransform_.rotation_.y),0,-sin(worldTransform_.rotation_.y),0,
	0,1,0,0,
	sin(worldTransform_.rotation_.y),0,cos(worldTransform_.rotation_.y),0,
	0,0,0,1
	};

	matRot = {
	1,0,0,0,
	0,1,0,0,
	0,0,1,0,
	0,0,0,1
	};
	//各軸の回転行列を合成
	matRot *= matRotZ;
	matRot *= matRotX;
	matRot *= matRotY;

	//MatrixIdentity(viewWorldMatRot);
	//viewWorldMatRot *= matRot;

	worldTransform_.matWorld_ = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};
	worldTransform_.matWorld_ *= matScale;
	worldTransform_.matWorld_ *= matRot;
	worldTransform_.matWorld_ *= matTrans;
}

int GameScene::MatrixInverse(Matrix4& pOut, Matrix4& pM)
{
	Matrix4 mat;
	int i, j, loop;
	double fDat, fDat2;
	double mat_8x4[4][8];
	int flag;
	float* pF;
	double* pD;

	//8 x 4行列に値を入れる
	for (i = 0; i < 4; i++) {
		pF = pM.m[i];
		for (j = 0; j < 4; j++, pF++) mat_8x4[i][j] = (double)(*pF);
		pD = mat_8x4[i] + 4;
		for (j = 0; j < 4; j++) {
			if (i == j)   *pD = 1.0;
			else         *pD = 0.0;
			pD++;
		}
	}

	flag = 1;
	for (loop = 0; loop < 4; loop++) {
		fDat = mat_8x4[loop][loop];
		if (fDat != 1.0) {
			if (fDat == 0.0) {
				for (i = loop + 1; i < 4; i++) {
					fDat = mat_8x4[i][loop];
					if (fDat != 0.0) break;
				}
				if (i >= 4) {
					flag = 0;
					break;
				}
				//行を入れ替える
				for (j = 0; j < 8; j++) {
					fDat = mat_8x4[i][j];
					mat_8x4[i][j] = mat_8x4[loop][j];
					mat_8x4[loop][j] = fDat;
				}
				fDat = mat_8x4[loop][loop];
			}

			for (i = 0; i < 8; i++) mat_8x4[loop][i] /= fDat;
		}
		for (i = 0; i < 4; i++) {
			if (i != loop) {
				fDat = mat_8x4[i][loop];
				if (fDat != 0.0f) {
					//mat[i][loop]をmat[loop]の行にかけて
					//(mat[j] - mat[loop] * fDat)を計算
					for (j = 0; j < 8; j++) {
						fDat2 = mat_8x4[loop][j] * fDat;
						mat_8x4[i][j] -= fDat2;
					}
				}
			}
		}
	}

	if (flag) {
		for (i = 0; i < 4; i++) {
			pF = mat.m[i];
			pD = mat_8x4[i] + 4;
			for (j = 0; j < 4; j++) {
				*pF = (float)(*pD);
				pF++;
				pD++;
			}
		}
	}
	else {
		//単位行列を求める
		mat = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
		};
	}

	pOut = mat;

	if (flag) return 1;
	return 0;
}

void GameScene::MatrixIdentity(Matrix4& pOut)
{
	int y;
	float* p1;

	for (y = 0; y < 4; y++) {
		p1 = pOut.m[y];
		*p1 = 0.0f;
		*(p1 + 1) = 0.0f;
		*(p1 + 2) = 0.0f;
		*(p1 + 3) = 0.0f;
	}
	pOut.m[0][0] = 1.0f;
	pOut.m[1][1] = 1.0f;
	pOut.m[2][2] = 1.0f;
	pOut.m[3][3] = 1.0f;
}

Matrix4 GameScene::MakeInverse(const Matrix4* mat)
{
	assert(mat);

	//掃き出し法を行う行列
	float sweep[4][8]{};
	//定数倍用
	float constTimes = 0.0f;
	//許容する誤差
	float MAX_ERR = 1e-10f;
	//戻り値用
	Matrix4 retMat;

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			//weepの左側に逆行列を求める行列をセット
			sweep[i][j] = mat->m[i][j];

			//sweepの右側に単位行列をセット
			sweep[i][4 + j] = MakeIdentity().m[i][j];
		}
	}

	//全ての列の対角成分に対する繰り返し
	for (int i = 0; i < 4; i++)
	{
		//最大の絶対値を注目対角成分の絶対値と仮定
		float max = std::fabs(sweep[i][i]);
		int maxIndex = i;

		//i列目が最大の絶対値となる行を探す
		for (int j = i + 1; j < 4; j++)
		{
			if (std::fabs(sweep[j][i]) > max)
			{
				max = std::fabs(sweep[j][i]);
				maxIndex = j;
			}
		}

		if (fabs(sweep[maxIndex][i]) <= MAX_ERR)
		{
			//逆行列は求められない
			return MakeIdentity();
		}

		//操作(1):i行目とmaxIndex行目を入れ替える
		if (i != maxIndex)
		{
			for (int j = 0; j < 8; j++)
			{
				float tmp = sweep[maxIndex][j];
				sweep[maxIndex][j] = sweep[i][j];
				sweep[i][j] = tmp;
			}
		}

		//sweep[i][i]に掛けると1になる値を求める
		constTimes = 1 / sweep[i][i];

		//操作(2):p行目をa倍する
		for (int j = 0; j < 8; j++)
		{
			//これによりsweep[i][i]が1になる
			sweep[i][j] *= constTimes;
		}

		//操作(3)によりi行目以外の行のi列目を0にする
		for (int j = 0; j < 4; j++)
		{
			if (j == i)
			{
				//i行目はそのまま
				continue;
			}

			//i行目に掛ける値を求める
			constTimes = -sweep[j][i];

			for (int k = 0; k < 8; k++)
			{
				//j行目にi行目をa倍した行を足す
				//これによりsweep[j][i]が0になる
				sweep[j][k] += sweep[i][k] * constTimes;
			}
		}
	}

	//sweepの右半分がmatの逆行列
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			retMat.m[i][j] = sweep[i][4 + j];
		}
	}

	return retMat;
}

Matrix4 GameScene::MakeIdentity()
{
	Matrix4 mat;
	MatrixIdentity(mat);
	return mat;
}

void GameScene::matRotCopy(WorldTransform& worldTransform_, Matrix4& pOut)
{
	Matrix4 matRot;
	Matrix4 matRotX;
	Matrix4 matRotY;
	Matrix4 matRotZ;

	//Z用回転軸の設定
	matRotZ = {
	cos(worldTransform_.rotation_.z),sin(worldTransform_.rotation_.z),0,0,
	-sin(worldTransform_.rotation_.z),cos(worldTransform_.rotation_.z),0,0,
	0,0,1,0,
	0,0,0,1
	};

	//X用回転軸の設定
	matRotX = {
	1,0,0,0,
	0,cos(worldTransform_.rotation_.x),sin(worldTransform_.rotation_.x),0,
	0,-sin(worldTransform_.rotation_.x),cos(worldTransform_.rotation_.x),0,
	0,0,0,1
	};

	//Y用回転軸の設定
	matRotY = {
	cos(worldTransform_.rotation_.y),0,-sin(worldTransform_.rotation_.y),0,
	0,1,0,0,
	sin(worldTransform_.rotation_.y),0,cos(worldTransform_.rotation_.y),0,
	0,0,0,1
	};

	matRot = {
	1,0,0,0,
	0,1,0,0,
	0,0,1,0,
	0,0,0,1
	};
	//各軸の回転行列を合成
	matRot *= matRotZ;
	matRot *= matRotX;
	matRot *= matRotY;

	pOut = matRot;
}

Matrix4 GameScene::MatrixLookAtlH(Matrix4& pOut, Vector3* TargetPos, Vector3* playerPos, Vector3* pUp)
{
	Vector3 zaxis;
	Vector3 xaxis;
	Vector3 yaxis;


	zaxis.x = { playerPos->x - TargetPos->x };
	zaxis.y = { playerPos->y - TargetPos->y };
	zaxis.z = { playerPos->z - TargetPos->z };

	Vec3Normalize(&zaxis, &zaxis);
	/*Vec3Normalize(&yaxis, pUp);*/
	Vec3Cross(&xaxis, pUp, &zaxis);
	Vec3Normalize(&xaxis, &xaxis);
	Vec3Cross(&yaxis, &zaxis, &xaxis);
	Vec3Normalize(&yaxis, &yaxis);

	//if (intterProdict < 0) {
		pOut.m[0][0] = xaxis.x; pOut.m[0][1] = xaxis.y; pOut.m[0][2] = xaxis.z; pOut.m[0][3] = 0;
		pOut.m[1][0] = yaxis.x; pOut.m[1][1] = yaxis.y; pOut.m[1][2] = yaxis.z; pOut.m[1][3] = 0;
		pOut.m[2][0] = zaxis.x; pOut.m[2][1] = zaxis.y; pOut.m[2][2] = zaxis.z; pOut.m[2][3] = 0;
		pOut.m[3][0] = 0; pOut.m[3][1] = 0; pOut.m[3][2] = 0; pOut.m[3][3] = 1;
	//}
	/*else
	{
		pOut.m[0][0] = -xaxis.x; pOut.m[0][1] = -xaxis.y; pOut.m[0][2] = -xaxis.z; pOut.m[0][3] = 0;
		pOut.m[1][0] = -yaxis.x; pOut.m[1][1] = -yaxis.y; pOut.m[1][2] = -yaxis.z; pOut.m[1][3] = 0;
		pOut.m[2][0] = -zaxis.x; pOut.m[2][1] = -zaxis.y; pOut.m[2][2] = -zaxis.z; pOut.m[2][3] = 0;
		pOut.m[3][0] = 0; pOut.m[3][1] = 0; pOut.m[3][2] = 0; pOut.m[3][3] = 1;
	}*/
	//pOut.m[0][0] = xaxis.x; pOut.m[0][1] = yaxis.x; pOut.m[0][2] = zaxis.x; pOut.m[0][3] = 0;
	//pOut.m[1][0] = xaxis.y; pOut.m[1][1] = yaxis.y; pOut.m[1][2] = zaxis.y; pOut.m[1][3] = 0;
	//pOut.m[2][0] = xaxis.z; pOut.m[2][1] = yaxis.z; pOut.m[2][2] = zaxis.z; pOut.m[2][3] = 0;
	//pOut.m[3][0] = -Vec3Dot(&xaxis,&eyes); pOut.m[3][1] = -Vec3Dot(&yaxis,&eyes); pOut.m[3][2] = -Vec3Dot(&zaxis,&eyes); pOut.m[3][3] = 1;

	return pOut;
}

int GameScene::Normalize(Vector3* pV)
{
	double len;
	double x, y, z;

	x = (double)(pV->x);
	y = (double)(pV->y);
	z = (double)(pV->z);
	len = sqrt(x * x + y * y + z * z);

	if (len < (1e-6)) return 0;

	len = 1.0 / len;
	x *= len;
	y *= len;
	z *= len;

	return 1;
}

void GameScene::GetInvRotateMat(Vector3* BillPos, Vector3* Targetpos, Matrix4& Rot,Vector3 up)
{

	MatrixIdentity(Rot);
	MatrixLookAtlH(Rot, Targetpos, BillPos, &up);
	/*MatrixInverse(Rot, Rot);*/
	MakeInverse(&Rot);

	Rot.m[3][0] = 0.0f;
	Rot.m[3][1] = 0.0f;
	Rot.m[3][2] = 0.0f;
}

void GameScene::AfinTransform(WorldTransform& worldTransform_)
{
	Matrix4 matTrans;
	matTrans = {
	1,0,0,0,
	0,1,0,0,
	0,0,1,0,
	worldTransform_.translation_.x,worldTransform_.translation_.y,worldTransform_.translation_.z,1
	};

	//worldTransform_.matWorld_ = {
	//	1,0,0,0,
	//	0,1,0,0,
	//	0,0,1,0,
	//	0,0,0,1
	//};
	worldTransform_.matWorld_ *= matTrans;
}

void GameScene::AfinMatRot(WorldTransform& worldTransform_, Matrix4& cameraRot)
{
	Matrix4 matRot;

	matRot = {
	1,0,0,0,
	0,1,0,0,
	0,0,1,0,
	0,0,0,1
	};
	//各軸の回転行列を合成
	matRot *= cameraRot;

	worldTransform_.matWorld_ *= matRot;
}

void GameScene::WorldVector(WorldTransform& worldTransform_, Vector3 direction)
{
	worldTransform_.TransferMatrix();

}

