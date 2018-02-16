#include "Player.h"

Player::Player(Vec2 & pos, Vec2 & dir)
	:
	pos(pos)
{
}

void Player::Update(float dt, const Keyboard& kbd)
{
	if (kbd.KeyIsPressed('A'))
	{
		pos += Vec2(-1, 0) * 5.0f;
	}
	if (kbd.KeyIsPressed('S'))
	{
		pos += Vec2(0, 1) * 5.0f;
	}
	if (kbd.KeyIsPressed('D'))
	{
		pos += Vec2(1, 0) * 5.0f;
	}
	if (kbd.KeyIsPressed('W'))
	{
		pos += Vec2(0, -1) * 5.0f;
	}
}


void Player::Draw(Graphics & gfx)
{
	gfx.DrawCircle((int)pos.x, (int)pos.y, radius, c);
}
