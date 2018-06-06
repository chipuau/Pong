//Megan Chipman
//PID: m3700960

#pragma once

#include "DrawableGameComponent.h"
#include "Game.h"
#include "Rectangle.h"
#include <d3d11_2.h>
#include <DirectXMath.h>
#include <wrl.h>

namespace BouncingBall
{
	class OpponentPaddle final : public Library::DrawableGameComponent
	{

	public:

		OpponentPaddle(Library::Game& game); 

		const Library::Rectangle& Bounds() const; 
		
		virtual void Initialize() override;  
		virtual void Draw(const Library::GameTime& gameTime) override; 
		virtual void UpdateUp();
		virtual void UpdateDown(); 
		void Reset();

	private:

		static const int PaddleSpeed; 

		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mTexture;
		Library::Point mTextureHalfSize;
		Library::Rectangle mBounds;
		DirectX::XMFLOAT2 mVelocity;
	};
}