//Megan Chipman
//PID: m3700960

#pragma once

#include "Game.h"

namespace Library
{
	class KeyboardComponent;
}

namespace BouncingBall
{
	class Ball;
	class PlayerPaddle;
	class OpponentPaddle;

	class BouncingBallGame : public Library::Game
	{

	public:

		BouncingBallGame(std::function<void*()> getWindowCallback, std::function<void(SIZE&)> getRenderTargetSizeCallback);

		virtual void Initialize() override;
		virtual void Shutdown() override;
		virtual void Update(const Library::GameTime& gameTime) override;
		virtual void Draw(const Library::GameTime& gameTime) override;

	private:

		void Exit();

		static const DirectX::XMVECTORF32 BackgroundColor;
		static const std::wstring Message; 

		std::shared_ptr<Library::KeyboardComponent> mKeyboard;

		std::shared_ptr<Ball> mBall;
		std::shared_ptr<PlayerPaddle>mPaddle;
		std::shared_ptr<OpponentPaddle>mPaddle2;

		std::shared_ptr<DirectX::SpriteFont> mFont;
		std::shared_ptr<DirectX::SpriteFont> mFont2;

		DirectX::XMFLOAT2 mMessagePosition;

		std::wstring mSubMessage;
		std::wstring mSubMessage2;
		std::wstring mSubMessage3;
		std::wstring mSubMessage4;

		DirectX::XMFLOAT2 mSubMessagePosition;
		DirectX::XMFLOAT2 mSubMessagePosition2;
		DirectX::XMFLOAT2 mSubMessagePosition3;
		DirectX::XMFLOAT2 mSubMessagePosition4;

		std::unique_ptr<DirectX::SoundEffect> mScore;
		std::unique_ptr<DirectX::SoundEffect> mScoreAgainst;
		std::unique_ptr<DirectX::SoundEffect> mRebound;
		std::unique_ptr<DirectX::SoundEffect> mGameOver;
		std::unique_ptr<DirectX::SoundEffect> mVictory;
		std::unique_ptr<DirectX::SoundEffect> mMusic;
		
		std::unique_ptr<DirectX::SoundEffectInstance> mMusicSound;
	};
}