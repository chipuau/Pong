//Megan Chipman
//PID: m3700960

#include "pch.h"
#include "Ball.h"

using namespace DirectX;
using namespace Library;
using namespace std;
using namespace Microsoft::WRL;

namespace BouncingBall
{
	//Declare and initialize speed of the ball:
	const int Ball::MinBallSpeed = 200;
	const int Ball::MaxBallSpeed = 205;

	//Create the device that determines what direction the ball starts with: 
	random_device Ball::sDevice;
	default_random_engine Ball::sGenerator(sDevice());
	uniform_int_distribution<int> Ball::sBoolDistribution(0, 1);
	uniform_int_distribution<int> Ball::sSpeedDistribution(MinBallSpeed, MaxBallSpeed);

	//Declare Ball Constructor: 
	Ball::Ball(Game& game) :
		DrawableGameComponent(game), mBounds(Rectangle::Empty)
	{
	}

	//Returns the bounds of the ball object: 
	const Library::Rectangle& Ball::Bounds() const
	{
		return mBounds;
	}

	//Returns the velocity of the ball object: 
	DirectX::XMFLOAT2& Ball::Velocity()
	{
		return mVelocity;
	}

	//Initialize the ball object: 
	void Ball::Initialize()
	{
		// Load and apply the ball texture: 
		ComPtr<ID3D11Resource> textureResource;
		wstring textureName = L"Content\\Textures\\Ball.png";

		ThrowIfFailed(CreateWICTextureFromFile(mGame->Direct3DDevice(), textureName.c_str(), textureResource.ReleaseAndGetAddressOf(), mTexture.ReleaseAndGetAddressOf()), "CreateWICTextureFromFile() failed.");

		ComPtr<ID3D11Texture2D> texture;
		ThrowIfFailed(textureResource.As(&texture), "Invalid ID3D11Resource returned from CreateWICTextureFromFile. Should be a ID3D11Texture2D.");

		mBounds = TextureHelper::GetTextureBounds(texture.Get());
		mTextureHalfSize.X = mBounds.Width / 2;
		mTextureHalfSize.Y = mBounds.Height / 2;

		Reset();
	}

	//Updates the ball object: 
	void Ball::Update(const Library::GameTime& gameTime)
	{
		float elapsedTime = gameTime.ElapsedGameTimeSeconds().count();

		//Calculate the X and Y positions of the ball: 
		XMFLOAT2 positionDelta(mVelocity.x * elapsedTime, mVelocity.y * elapsedTime);
		mBounds.X += static_cast<int>(std::round(positionDelta.x));
		mBounds.Y += static_cast<int>(std::round(positionDelta.y));

		//Check to see if the ball collides with the screen edges.
		//      If it does, change the direction of the ball accordingly.
		auto& viewport = mGame->Viewport();
		if (mBounds.X + mBounds.Width >= viewport.Width && mVelocity.x > 0.0f)
		{
			mVelocity.x *= -1;
		}
		if (mBounds.X <= 0 && mVelocity.x < 0.0f)
		{
			mVelocity.x *= -1;
		}

		if (mBounds.Y + mBounds.Height >= viewport.Height && mVelocity.y > 0.0f)
		{
			mVelocity.y *= -1;
		}
		if (mBounds.Y <= 0 && mVelocity.y < 0.0f)
		{
			mVelocity.y *= -1;
		}
		
	}
	
	//Changes the velocity and direction of the ball whenever it hits a paddle:
	void Ball::DetectHit() {
		if (mVelocity.x < MaxBallSpeed)
		{
			mVelocity.x *= -1.25;
			mVelocity.y = static_cast<float>(mVelocity.y * 1.25 * (sBoolDistribution(sGenerator) ? 1 : -1));
		}

		else
		{
			mVelocity.x *= -1;
			mVelocity.y = static_cast<float>(mVelocity.y * 1.25 * (sBoolDistribution(sGenerator) ? 1 : -1));
		}
	}

	//Draws the ball sprite: 
	void Ball::Draw(const Library::GameTime& gameTime)
	{
		UNREFERENCED_PARAMETER(gameTime);

		XMFLOAT2 position(static_cast<float>(mBounds.X), static_cast<float>(mBounds.Y));
		SpriteManager::DrawTexture2D(mTexture.Get(), position);
	}

	//Resets the ball: 
	void Ball::Reset()
	{
		Library::Rectangle viewportSize(static_cast<int>(mGame->Viewport().TopLeftX), static_cast<int>(mGame->Viewport().TopLeftY), static_cast<int>(mGame->Viewport().Width), static_cast<int>(mGame->Viewport().Height));
		Point center = viewportSize.Center();
		mBounds.X = center.X - mTextureHalfSize.X;
		mBounds.Y = center.Y - mTextureHalfSize.Y;

		mVelocity.x = static_cast<float>(MinBallSpeed * (sBoolDistribution(sGenerator) ? 1 : -1));
		mVelocity.y = static_cast<float>(MinBallSpeed * (sBoolDistribution(sGenerator) ? 1 : -1));
	}
}