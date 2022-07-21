#pragma once


#include "Audio.h"
#include "DirectXCommon.h"
#include "DebugText.h"
#include "Input.h"
#include "Model.h"
#include "SafeDelete.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "DebugCamera.h"
#include "AxisIndicator.h"
#include <math.h>
#include "MathUtility.h"
/// <summary>
/// ゲームシーン
/// </summary>
class GameScene {

public: // メンバ関数
  /// <summary>
  /// コンストクラタ
  /// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();


	//ベクトルを正規化する
	int Vec3Normalize(Vector3* pOut, Vector3* pV);
	//ベクトルの内積を求める
	float Vec3Dot(Vector3* pV1, Vector3* pV2);
	//ベクトルの外積を求める
	void Vec3Cross(Vector3* pOut, Vector3* pV1, Vector3* pV2);

	void Afin(WorldTransform& worldTransform_, Matrix4& viewWorldMatRot);

	int MatrixInverse(Matrix4& pOut, Matrix4& pM);

	void MatrixIdentity(Matrix4& pOut);

	Matrix4 MakeInverse(const Matrix4* mat);

	Matrix4 MakeIdentity();

	void matRotCopy(WorldTransform& worldTransform_, Matrix4& pOut);

	Matrix4 MatrixLookAtlH(Matrix4& pOut, Vector3* Eye, Vector3* At, Vector3* pUp);

	int Normalize(Vector3* pV);

	void GetInvRotateMat(Vector3* worldPos, Vector3* Targetpos,Matrix4& Rot,Vector3 up);

	void AfinTransform(WorldTransform& worldTransform_);

	void AfinMatRot(WorldTransform& worldTransform_,Matrix4& cameraRot);

	void WorldVector(WorldTransform& worldTransform_,Vector3 direction);


private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;
	DebugText* debugText_ = nullptr;
	Model* model_ = nullptr;
	Model* pModel_ = nullptr;

	//頂点の配列
	Vector3 vertex_[4] = {
		{-5,-5,0},
		{5,-5,0},
		{-5,5,0},
		{5,5,0},
	};

	int eageList_[4][2] =
	{
		{0,1},
		{0,2},
		{1,3},
		{2,3}
	};
	Vector3  vv1 = { 0,0,0 };//第一ベクトル
	Vector3  vv2 = { 0,0,0 };//第二ベクトル
	Vector3  n1 = { 0,0,0 };//法線ベクトル

	Vector3  eyes = { 0,0,0 };//視点ベクトル
	Vector3  trans = { 0,0,0 };//視点ベクトル
	Vector3  up = { 0,1,0 };//視点ベクトル

	Vector3 viewP={0,0,0};

	//ワールドトランスフォーム
	WorldTransform worldTransform_[3];
	WorldTransform viewTransForm;
	//ビュープロジェクション
	ViewProjection viewProjection_;
	//テクスチャハンドル
	uint32_t textureHandle_ = 0;

	/// <summary>
	/// ゲームシーン用
	/// </summary>
	/// 
	//デバックカメラ
	DebugCamera* debugCamera_ = nullptr;


	//内積を保存する変数
	float intterProdict = 0;

	//描画するときのviewProjectionを変更
	int viewSelct = 0;

	//カメラの逆行列を保存する変数
	Matrix4 inverseMatrix;

	////ビュー行列を入れる変数
	Matrix4 viewContainer;

	Matrix4 viewContainer2;
	

};
