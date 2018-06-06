//Megan Chipman
//PID: m3700960

#include "pch.h"
#include "BouncingBallGame.h"
#include "Ball.h"
#include "PlayerPaddle.h"
#include "OpponentPaddle.h"

using namespace std;
using namespace DirectX;
using namespace Library;
using namespace Microsoft::WRL;

namespace BouncingBall
{
	//Declare and Initialize Constants and Variables: 
	const XMVECTORF32 BouncingBallGame::BackgroundColor = Colors::CadetBlue;
	const wstring BouncingBallGame::Message = L"Pong";
	int mPlayerScore = 0; 
	int mOpponentScore = 0; 
	int flag = 0;

	//Declare Constructor for BouncingBallGame: 
	BouncingBallGame::BouncingBallGame(function<void*()> getWindowCallback, function<void(SIZE&)> getRenderTargetSizeCallback) :
		Game(getWindowCallback, getRenderTargetSizeCallback)
	{
	}

	//Initialize BouncingBallGame and its Components: 
	void BouncingBallGame::Initialize()
	{

		SpriteManager::Initialize(*this);
		BlendStates::Initialize(mDirect3DDevice.Get());

		//Create Audio Engine to Help with Audio Components: 
		auto audioEngine = make_shared<AudioEngineComponent>(*this); 
		mComponents.push_back(audioEngine); 
		mServices.AddService(AudioEngineComponent::TypeIdClass(), audioEngine.get());

		//Declare and Link Sound Effects and Music: 
		mScore = make_unique<SoundEffect>(audioEngine->AudioEngine().get(), L"Content\\Audio\\clapping.wav");
		mScoreAgainst = make_unique<SoundEffect>(audioEngine->AudioEngine().get(), L"Content\\Audio\\LosePoint.wav");
		mGameOver = make_unique<SoundEffect>(audioEngine->AudioEngine().get(), L"Content\\Audio\\GameOver.wav");
		mRebound = make_unique<SoundEffect>(audioEngine->AudioEngine().get(), L"Content\\Audio\\pop.wav");
		mVictory = make_unique<SoundEffect>(audioEngine->AudioEngine().get(), L"Content\\Audio\\Victory.wav");

		mMusic = make_unique<SoundEffect>(audioEngine->AudioEngine().get(), L"Content\\Audio\\loop.wav");
		mMusicSound = mMusic->CreateInstance();

		//Declare and Initialize Keyboard Component: 
		mKeyboard = make_shared<KeyboardComponent>(*this);
		mComponents.push_back(mKeyboard);
		mServices.AddService(KeyboardComponent::TypeIdClass(), mKeyboard.get());

		//Declare and Initialize Ball Component: 
		mBall = make_shared<Ball>(*this);
		mComponents.push_back(mBall);

		//Declare and Initialize the User Controlled Paddle: 
		mPaddle = make_shared<PlayerPaddle>(*this); 
		mComponents.push_back(mPaddle); 

		//Declare and Initialize the AI Paddle: 
		mPaddle2 = make_shared<OpponentPaddle>(*this); 
		mComponents.push_back(mPaddle2);

		//Declare and Initialize Fonts: 
		mFont = make_shared<SpriteFont>(mDirect3DDevice.Get(), L"Content\\Fonts\\Arial_36_Regular.spritefont");
		mFont2 = make_shared<SpriteFont>(mDirect3DDevice.Get(), L"Content\\Fonts\\Arial_14_Regular.spritefont");

		Game::Initialize();
	}

	//Shuts down BouncingBallGame: 
	void BouncingBallGame::Shutdown()
	{
		BlendStates::Shutdown();
		SpriteManager::Shutdown();
	}

	//Updates BouncingBallGame: 
	void BouncingBallGame::Update(const GameTime &gameTime)
	{
		//Begin Looping Music: 
		mMusicSound->Play(true);

		//Exit the game when the exit key is pressed:
		if (mKeyboard->WasKeyPressedThisFrame(Keys::Escape))
		{
			Exit();
		}

		//If the Up key is pressed, then move the user controlled paddle up. 
		//If the Down key is pressed, then move the user controlled paddle down.
		if (mKeyboard->WasKeyDown(Keys::Up))
		{
			mPaddle->UpdateUp(); 
		}

		if (mKeyboard->WasKeyDown(Keys::Down))
		{
			mPaddle->UpdateDown(); 
		}

		//If the Ball is moving upward, then move the AI paddle upward.
		//If the Ball is moving downward, then move the AI paddle downward.
		if (mBall->Velocity().y < 0)
		{
			mPaddle2->UpdateUp(); 
		}

		if (mBall->Velocity().y > 0)
		{
			mPaddle2->UpdateDown(); 
		}

		//If the ball makes contact with one of the paddles, then play the rebound 
		//   noise and change the direction of the ball accordingly. 
		if (mBall->Bounds().Intersects(mPaddle->Bounds()))
		{
			mRebound->Play();
			mBall->DetectHit();
		}

		if (mBall->Bounds().Intersects(mPaddle2->Bounds())) 
		{
			mRebound->Play();
			mBall->DetectHit(); 
		}

		//Once the ball goes beyond the reach of each paddle, reset the ball, play the appropriate sound,
		//     and update the scores.
		if (mBall->Bounds().X < mPaddle->Bounds().X)
		{
			mScoreAgainst->Play();
			mOpponentScore += 1; 
			mBall->Reset();
			mPaddle->Reset(); 
			mPaddle2->Reset(); 
		}

		if (mBall->Bounds().X > mPaddle2->Bounds().X)
		{
			mScore->Play(); 
			mPlayerScore += 1; 
			mBall->Reset();
			mPaddle->Reset(); 
			mPaddle2->Reset(); 
		}

		//Declare size of the Viewport using Height and Width
		XMFLOAT2 tempViewportSize(mViewport.Width, mViewport.Height);
		XMVECTOR viewportSize = XMLoadFloat2(&tempViewportSize);


		//Measure the size of the message, and position it. 
		XMVECTOR messageSize = mFont->MeasureString(Message.c_str());
		XMStoreFloat2(&mMessagePosition, (viewportSize - messageSize) / 2);
		mMessagePosition.y = 50;
		mMessagePosition.x = 370; 

		//Declare and Initialize submessages:
		//Player's Score: 
		wostringstream subMessageStream;
		subMessageStream << mPlayerScore; 
		mSubMessage = subMessageStream.str();
		messageSize = mFont->MeasureString(mSubMessage.c_str());
		XMStoreFloat2(&mSubMessagePosition, (viewportSize - messageSize) / 2);
		mSubMessagePosition.x = 305;
		mSubMessagePosition.y = 50;

		//Opponent's Score: 
		wostringstream subMessageStream2;
		subMessageStream2 << mOpponentScore;
		mSubMessage2 = subMessageStream2.str();
		messageSize = mFont->MeasureString(mSubMessage2.c_str());
		XMStoreFloat2(&mSubMessagePosition2, (viewportSize - messageSize) / 2);
		mSubMessagePosition2.x = 520;
		mSubMessagePosition2.y = 50;

		//If either score reaches 5, then play the appropriate sound, show the appropriate message, 
		//   and stop gameplay: 
		//Opponent Wins: 
		if (mOpponentScore >= 5)
		{
			if (flag == 0) {
				mGameOver->Play();
				flag = 1;
			}

			wostringstream subMessageStream3;
			subMessageStream3 << "Game Over";
			mSubMessage3 = subMessageStream3.str();
			messageSize = mFont->MeasureString(mSubMessage3.c_str());
			XMStoreFloat2(&mSubMessagePosition3, (viewportSize - messageSize) / 2);
			mSubMessagePosition3.x = mMessagePosition.x - 60;
			mSubMessagePosition3.y = mMessagePosition.y + 70;

			wostringstream subMessageStream4;
			subMessageStream4 << "Press Enter to Restart";
			mSubMessage4 = subMessageStream4.str();
			messageSize = mFont2->MeasureString(mSubMessage4.c_str());
			XMStoreFloat2(&mSubMessagePosition4, (viewportSize - messageSize) / 2);
			mSubMessagePosition4.x = mMessagePosition.x - 35;
			mSubMessagePosition4.y = mMessagePosition.y + 300;

			mBall->Velocity().x = 0;
			mBall->Velocity().y = 0;
		}

		//Player Wins: 
		if (mPlayerScore >= 5)
		{
			if (flag == 0) {
				mVictory->Play();
				flag = 1;
			}

			wostringstream subMessageStream3;
			subMessageStream3 << "You Won!";
			mSubMessage3 = subMessageStream3.str();
			messageSize = mFont->MeasureString(mSubMessage3.c_str());
			XMStoreFloat2(&mSubMessagePosition3, (viewportSize - messageSize) / 2);
			mSubMessagePosition3.x = mMessagePosition.x - 50;
			mSubMessagePosition3.y = mMessagePosition.y + 70;

			wostringstream subMessageStream4;
			subMessageStream4 << "Press Enter to Restart";
			mSubMessage4 = subMessageStream4.str();
			messageSize = mFont2->MeasureString(mSubMessage4.c_str());
			XMStoreFloat2(&mSubMessagePosition4, (viewportSize - messageSize) / 2);
			mSubMessagePosition4.x = mMessagePosition.x - 10;
			mSubMessagePosition4.y = mMessagePosition.y + 300;

			mBall->Velocity().x = 0;
			mBall->Velocity().y = 0;
		}

		//Press Enter to Restart the Game: 
		if (mKeyboard->WasKeyPressedThisFrame(Keys::Enter))
		{
			mBall->Reset(); 
			mPaddle->Reset(); 
			mPaddle2->Reset();
			mPlayerScore = 0;
			mOpponentScore = 0;
			mSubMessage4.clear();
			mSubMessage3.clear();
			flag = 0;
		}

		//Play rebound noise if the ball hits the top and bottom edges of the screen. 
		if (mBall->Bounds().Y + mBall->Bounds().Height >= Viewport().Height)
		{
			mRebound->Play();
		}

		if (mBall->Bounds().Y <= 0)
		{
			mRebound->Play();
		}

		Game::Update(gameTime);
	}

	//Draws Game Components and Text: 
	void BouncingBallGame::Draw(const GameTime &gameTime)
	{
		mDirect3DDeviceContext->ClearRenderTargetView(mRenderTargetView.Get(), reinterpret_cast<const float*>(&BackgroundColor));
		mDirect3DDeviceContext->ClearDepthStencilView(mDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
				
		Game::Draw(gameTime);

		//Draw the Strings: 
		SpriteManager::DrawString(mFont, Message.c_str(), mMessagePosition);
		SpriteManager::DrawString(mFont, mSubMessage.c_str(), mSubMessagePosition);
		SpriteManager::DrawString(mFont, mSubMessage2.c_str(), mSubMessagePosition2);
		SpriteManager::DrawString(mFont, mSubMessage3.c_str(), mSubMessagePosition3);
		SpriteManager::DrawString(mFont2, mSubMessage4.c_str(), mSubMessagePosition4);


		HRESULT hr = mSwapChain->Present(1, 0);

		// If the device was removed either by a disconnection or a driver upgrade, we must recreate all device resources.
		if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
		{
			HandleDeviceLost();
		}
		else
		{
			ThrowIfFailed(hr, "IDXGISwapChain::Present() failed.");
		}
	}

	//Posts Quit Message: 
	void BouncingBallGame::Exit()
	{
		PostQuitMessage(0);
	}

}