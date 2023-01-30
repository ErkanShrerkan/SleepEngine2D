#include "pch.h"
#include "SplashScreen.h"
#include "Sprite.h"
#include "SpriteFactory.h"
#include "ContentLoader.h"
#include "Engine.h"
#include <iostream>
#include <Game\macro.h>

SplashScreen::SplashScreen()
{
	myFMODLogo = SE::CEngine::GetInstance()->GetContentLoader()->GetSpriteFactory().GetSprite("textures/sprites/FMODLogo.dds");
	myFMODLogo->SetPivot({ .5f, .5f });
	myFMODLogo->SetPosition({ 0.5f, 0.5f });
	myFMODLogo->SetColor({ 1, 1, 1, 0 });

	myTGALogo = SE::CEngine::GetInstance()->GetContentLoader()->GetSpriteFactory().GetSprite("textures/sprites/tga/tgalogo_W.dds");
	myTGALogo->SetPivot({ 0.5f, 0.5f });
	myTGALogo->SetPosition({ 0.5f, 0.5f });
	
	myGroupLogo = SE::CEngine::GetInstance()->GetContentLoader()->GetSpriteFactory().GetSprite("textures/sprites/GroupLogo.dds");
	myGroupLogo->SetPivot({ 0.5f, 0.5f });
	myGroupLogo->SetPosition({ 0.5f, 0.5f });
	myGroupLogo->SetColor({1, 1, 1, 0});

	myBackground = SE::CEngine::GetInstance()->GetContentLoader()->GetSpriteFactory().GetSprite("textures/black.dds");
	myBackground->SetPivot({ 0, 0 });
	myBackground->SetPosition({ 0, 0 });
	myBackground->SetSizeRelativeToScreen({ 1, 1 });
	
	myTGATimer.SetTimeInterval(4);
	myGroupTimer.SetTimeInterval(4);
	myFMODTimer.SetTimeInterval(4);
	
	myState = eState::TGA;
}

SplashScreen::~SplashScreen()
{
	myTGALogo->Release();
	myGroupLogo->Release();
	myFMODLogo->Release();
}

void SplashScreen::Update(float aDeltaTime)
{
	switch (myState)
	{
	case eState::TGA:
		UpdateTGA(aDeltaTime);
		break;
	case eState::Group:
		UpdateGroup(aDeltaTime);
		break;
	case eState::FMOD :
		UpdateFMOD(aDeltaTime);
		break;
	case eState::Over:
		break;
	}
}

void SplashScreen::Render()
{
	myBackground->Render();
	switch (myState)
	{
	case eState::TGA:
		myTGALogo->Render();
		break;
	case eState::Group:
		myGroupLogo->Render();
		break;
	case eState::FMOD:
		myFMODLogo->Render();
		break;
	case eState::Over:
		break;
	}
}

SplashScreen::eState SplashScreen::GetState()
{
	return myState;
}

void SplashScreen::UpdateTGA(float aDeltaTime)
{
	myTGATimer.Update(aDeltaTime);
	myTGAScale += aDeltaTime * 0.025f;
	myTGALogo->SetSizeRelativeToImage({ myTGAScale, myTGAScale });

	float fadePercent = .8f;
	float percent = myTGATimer.GetTime() / myTGATimer.GetTimeInterval();
	float timeToZero = myTGATimer.GetTimeInterval() * (1 - fadePercent);

	if (percent >= fadePercent)
	{
		myTGAAlpha = 1 - ((myTGATimer.GetTime() - myTGATimer.GetTimeInterval() * fadePercent) / timeToZero);
		myTGALogo->SetColor({ 1, 1, 1, myTGAAlpha });
	}

	if (myTGATimer.IsOver())
	{
		myState = eState::Group;
	}
}

void SplashScreen::UpdateGroup(float aDeltaTime)
{
	myGroupTimer.Update(aDeltaTime);
	myGroupScale += aDeltaTime * 0.025f;
	myGroupLogo->SetSizeRelativeToImage({ myGroupScale / 4, myGroupScale / 4 });

	float appearPercent = .2f;
	float fadePercent = .8f;
	float percent = myGroupTimer.GetTime() / myGroupTimer.GetTimeInterval();
	float timeToZero = myGroupTimer.GetTimeInterval() * (1 - fadePercent);
	float timeToOne = myGroupTimer.GetTimeInterval() * (1 - appearPercent);

	if (percent <= appearPercent)
	{
		myGroupAlpha = (myGroupTimer.GetTime() / (myGroupTimer.GetTimeInterval() - timeToOne));
		myGroupLogo->SetColor({ 1, 1, 1, myGroupAlpha });
	}
	if (percent >= fadePercent)
	{
		myGroupAlpha = 1 - ((myGroupTimer.GetTime() - myGroupTimer.GetTimeInterval() * fadePercent) / timeToZero);
		myGroupLogo->SetColor({ 1, 1, 1, myGroupAlpha });
	}

	if (myGroupTimer.IsOver())
	{
		myState = eState::FMOD;
	}
}

void SplashScreen::UpdateFMOD(float aDeltaTime)
{
	myFMODTimer.Update(aDeltaTime);
	myFMODScale += aDeltaTime * 0.0125f;
	myFMODLogo->SetSizeRelativeToImage({ myFMODScale, myFMODScale });

	float appearPercent = .2f;
	float fadePercent = .8f;
	float percent = myFMODTimer.GetTime() / myFMODTimer.GetTimeInterval();
	float timeToZero = myFMODTimer.GetTimeInterval() * (1 - fadePercent);
	float timeToOne = myFMODTimer.GetTimeInterval() * (1 - appearPercent);

	if (percent <= appearPercent)
	{
		myFMODAlpha = (myFMODTimer.GetTime() / (myFMODTimer.GetTimeInterval() - timeToOne));
		myFMODLogo->SetColor({ 1, 1, 1, myFMODAlpha });
	}
	if (percent >= fadePercent)
	{
		myFMODAlpha = 1 - ((myFMODTimer.GetTime() - myFMODTimer.GetTimeInterval() * fadePercent) / timeToZero);
		myFMODLogo->SetColor({ 1, 1, 1, myFMODAlpha });
	}

	if (myFMODTimer.IsOver())
	{
		myState = eState::Over;
	}
}
