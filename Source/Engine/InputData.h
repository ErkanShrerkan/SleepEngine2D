#pragma once

enum class eInputState
{
	Null		= 0,
	Neutral		= 1 << 0,
	Pressed		= 1 << 1,
	Held		= 1 << 2,
	Released	= 1 << 3
};

enum class eScrollState : uint
{
	Null,
	Neutral,
	Up, 
	Down,
};

inline eInputState operator|(eInputState lhs, eInputState rhs)
{
	return static_cast<eInputState>(static_cast<uint>(lhs) | static_cast<uint>(rhs));
}

inline eInputState operator&(eInputState lhs, eInputState rhs)
{
	return static_cast<eInputState>(static_cast<uint>(lhs) & static_cast<uint>(rhs));
}

enum class eInputEvent : uint
{
	Up,
	Down,
	Forward,
	Back,
	Left,
	Right,
	Escape,
	Jump,
	ToggleAllPasses,
	TogglePositionPass,
	ToggleAlbedoPass,
	ToggleNormalPass,
	ToggleVertexNormalPass,
	ToggleMaterialPass,
	ToggleAOPass,
	ToggleDepthPass,
	MovementToggle,
	RotUp,
	RotDown,
	RotLeft,
	RotRight,
	LMB,
	RMB,
	ChangeCamera,
	ToggleDrawLine,
	H,
	G,
	J,
	O,
	P,
	Interact,
	UseItem,
	Reload,
	One,
	Two,
	Three,
	Quit,
	Alt,
	F4,
	ScrollUp,
	ScrollDown,
	MMB,
	Shift,
	Q,
	W,
	E,
	R,

	COUNT
};