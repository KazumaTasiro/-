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
	eyes = { debugCamera_->GetViewProjection().eye.x - 0,debugCamera_->GetViewProjection().eye.y - 0,debugCamera_->GetViewProjection().eye.z - 0 };
	trans = { debugCamera_->GetViewProjection().target.x - 0,debugCamera_->GetViewProjection().target.y - 0,debugCamera_->GetViewProjection().target.z - 0 };
	up = { 0.0f,1.0f,0.0f };

	//カメラと法線の内積
	intterProdict = Vec3Dot(&n1, &eyes);

	//カメラと逆ベクトルを計算
	//MatrixIdentity(viewContainer2);
	//MatrixInverse(inverseMatrix, &viewProjection_.matView);



	MatrixIdentity(viewContainer);
	MatrixIdentity(viewContainer2);
	GetInvRotateMat(&worldTransform_[1].translation_, &eyes, viewContainer, intterProdict);
	MatrixInverse(viewContainer2, viewContainer);
	/*viewContainer2 *= viewContainer;*/
	MatrixIdentity(worldTransform_[1].matWorld_);
	//if (intterProdict < 0) {
	//	for (int i = 0; i < 4; i++) {
	//		for (int j = 0; j < 4; j++) {
	//			viewContainer.m[i][j] *= -viewContainer.m[i][j];
	//		}
	//	}
	//}
	worldTransform_[1].matWorld_ *= viewContainer2;
	worldTransform_[1].matWorld_ *= viewContainer;
	worldTransform_[1].matWorld_ *= viewProjection_.matProjection;
	/*worldTransform_[1].matWorld_ *= viewContainer2;*/
	//AfinMatRot(worldTransform_[1], viewContainer);
	AfinTransform(worldTransform_[1]);

	worldTransform_[1].TransferMatrix();

	if (input_->PushKey(DIK_A)) {
		viewSelct = 1;
	}
	else if (input_->PushKey(DIK_S)) {
		viewSelct = 0;
	}

	//内積の値
	debugText_->SetPos(50, 50);
	debugText_->Printf(
		"intterProduct:(%f)", intterProdict);
	//視点の座標
	debugText_->SetPos(50, 70);
	debugText_->Printf(
		"debugCameraPosition:(x %f\n,y %f\n,z %f\n)", eyes.x, eyes.y, eyes.z);
	//視点の座標
	debugText_->SetPos(50, 190);
	debugText_->Printf(
		"debugCameraPosition:(x %f\n,y %f\n,z %f\n)", trans.x, trans.y, trans.z);
	//視点の座標
	debugText_->SetPos(50, 170);
	debugText_->Printf(
		"debugCameraPosition:(x %f\n,y %f\n,z %f\n)", up.x, up.y, up.z);
	//表か裏か
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
		"%f%f%f", worldTransform_[1].translation_.x, worldTransform_[1].translation_.y, worldTransform_[1].translation_.z);

	if (viewSelct == 0) {
		debugText_->SetPos(50, 150);
		debugText_->Printf(
			"viewProjection:debugCamera");
	}
	else
	{
		debugText_->SetPos(50, 150);
		debugText_->Printf(
			"viewProjection:viewProjection");
	}
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
	model_->Draw(worldTransform_[0], debugCamera_->GetViewProjection(), textureHandle_);
	pModel_->Draw(worldTransform_[1], debugCamera_->GetViewProjection(), textureHandle_);


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

	//ライン描画が参照するビュープロジェクションを指定する（アドレス渡し）
	for (int i = 0; i < 4; i++) {
		PrimitiveDrawer::GetInstance()->DrawLine3d(vertex_[eageList_[i][0]], vertex_[eageList_[i][1]], Vector4(0xff, 0xff, 0xff, 0xff));
	}
	PrimitiveDrawer::GetInstance()->DrawLine3d(Vector3(0, 0, 0), n1, Vector4(0xff, 0xff, 0xff, 0xff));
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

Matrix4 GameScene::MatrixLookAtlH(Matrix4& pOut, Vector3* TargetPos, Vector3* playerPos, Vector3* pUp, float intterProdict)
{
	Vector3 zaxis;
	Vector3 xaxis;
	Vector3 yaxis;


	zaxis.x = { playerPos->x - TargetPos->x };
	zaxis.y = { playerPos->y - TargetPos->y };
	zaxis.z = { playerPos->z - TargetPos->z };
	Vec3Normalize(&zaxis, &zaxis);
	Vec3Normalize(&yaxis, pUp);
	Vec3Cross(&xaxis, &yaxis, &zaxis);
	Vec3Normalize(&xaxis, &xaxis);
	Vec3Cross(&yaxis, &zaxis, &xaxis);
	Vec3Normalize(&yaxis, &yaxis);


		pOut.m[0][0] = xaxis.x; pOut.m[0][1] = xaxis.y; pOut.m[0][2] = xaxis.z; pOut.m[0][3] = 0;
		pOut.m[1][0] = yaxis.x; pOut.m[1][1] = yaxis.y; pOut.m[1][2] = yaxis.z; pOut.m[1][3] = 0;
		pOut.m[2][0] = zaxis.x; pOut.m[2][1] = zaxis.y; pOut.m[2][2] = zaxis.z; pOut.m[2][3] = 0;
		pOut.m[3][0] = 0; pOut.m[3][1] = 0; pOut.m[3][2] = 0; pOut.m[3][3] = 1;

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

void GameScene::GetInvRotateMat(Vector3* worldPos, Vector3* Targetpos, Matrix4& Rot, float intterProdict)
{

	MatrixIdentity(Rot);
	MatrixLookAtlH(Rot, worldPos, Targetpos, &up, intterProdict);
	MatrixInverse(Rot, Rot);

	//Rot.m[3][0] = 0.0f;
	//Rot.m[3][1] = 0.0f;
	//Rot.m[3][2] = 0.0f;
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

