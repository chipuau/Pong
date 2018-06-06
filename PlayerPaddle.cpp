//Megan Chipman
//PID: m3700960

#include "pch.h"
#include "PlayerPaddle.h"

using namespace DirectX;
using namespace Library; 
using namespace std; 
using namespace Microsoft::WRL;

namespace BouncingBall
{
	//Constructor for the User Controlled Paddle: 
	PlayerPaddle::PlayerPaddle(Game& game) :
		DrawableGameComponent(game), mBounds(Rectangle::Empty)
	{
	}

	//Returns the bounds of the User Controlled Paddle: 
	const Library::Rectangle& PlayerPaddle::Bounds() const
	{
		return mBounds;
	}

	//Initializes the User Controlled Paddle: 
	void PlayerPaddle::Initialize()
	{
		//Link and apply a texture: 
		ComPtr<ID3D11Resource> textureResource; 
		wstring textureName = L"Content\\Textures\\Paddle.png"; 

		ThrowIfFailed(CreateWICTextureFromFile(mGame->Direct3DDevice(), textureName.c_str(), textureResource.ReleaseAndGetAddressOf(), mTexture.ReleaseAndGetAddressOf()), "CreateWICTextureFromFile() failed.");

		ComPtr<ID3D11Texture2D> texture;
		ThrowIfFailed(textureResource.As(&texture), "Invalid ID3D11Resource returned from CreateWICTextureFromFile. Should be a ID3D11Texture2D.");

		mBounds = TextureHelper::GetTextureBounds(texture.Get());
		mTextureHalfSize.X = mBounds.Width / 2;
		mTextureHalfSize.Y = mBounds.Height / 2;

		Reset();
	}

	//Moves the User Controlled Paddle either Up or Down: 
	void PlayerPaddle::UpdateUp()
	{
		if (mBounds.Y > 0)
		{
			mBounds.Y -= 3;
		}
	}

	void PlayerPaddle::UpdateDown()
	{
		auto& viewport = mGame->Viewport();
		if (mBounds.Y + mBounds.Height <= viewport.Height)
		{
			mBounds.Y += 3;
		}
	}

	//Draws the User Controlled Paddle: 
	void PlayerPaddle::Draw(const Library::GameTime& gameTime)
	{
		UNREFERENCED_PARAMETER(gameTime);

		XMFLOAT2 position(static_cast<float>(mBounds.X), static_cast<float>(mBounds.Y));
		SpriteManager::DrawTexture2D(mTexture.Get(), position);
	}

	//Resets the User Controlled Paddle: 
	void PlayerPaddle::Reset()
	{
		Library::Rectangle viewportSize(static_cast<int>(mGame->Viewport().TopLeftX), static_cast<int>(mGame->Viewport().TopLeftY), static_cast<int>(mGame->Viewport().Width), static_cast<int>(mGame->Viewport().Height));
		Point center = viewportSize.Center();
		mBounds.X = 20;
		mBounds.Y = center.Y - mTextureHalfSize.Y;

	}

}
