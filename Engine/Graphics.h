/******************************************************************************************
*	Chili DirectX Framework Version 16.07.20											  *
*	Graphics.h																			  *
*	Copyright 2016 PlanetChili <http://www.planetchili.net>								  *
*																						  *
*	This file is part of The Chili DirectX Framework.									  *
*																						  *
*	The Chili DirectX Framework is free software: you can redistribute it and/or modify	  *
*	it under the terms of the GNU General Public License as published by				  *
*	the Free Software Foundation, either version 3 of the License, or					  *
*	(at your option) any later version.													  *
*																						  *
*	The Chili DirectX Framework is distributed in the hope that it will be useful,		  *
*	but WITHOUT ANY WARRANTY; without even the implied warranty of						  *
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the						  *
*	GNU General Public License for more details.										  *
*																						  *
*	You should have received a copy of the GNU General Public License					  *
*	along with The Chili DirectX Framework.  If not, see <http://www.gnu.org/licenses/>.  *
******************************************************************************************/
#pragma once
#include "ChiliWin.h"
#include <d3d11.h>
#include <wrl.h>
#include "ChiliException.h"
#include "Colors.h"
#include "Surface.h"
#include "Rect.h"
#include <cassert>

class Graphics
{
public:
	class Exception : public ChiliException
	{
	public:
		Exception( HRESULT hr,const std::wstring& note,const wchar_t* file,unsigned int line );
		std::wstring GetErrorName() const;
		std::wstring GetErrorDescription() const;
		virtual std::wstring GetFullMessage() const override;
		virtual std::wstring GetExceptionType() const override;
	private:
		HRESULT hr;
	};
private:
	// vertex format for the framebuffer fullscreen textured quad
	struct FSQVertex
	{
		float x,y,z;		// position
		float u,v;			// texcoords
	};
public:
	Graphics( class HWNDKey& key );
	Graphics( const Graphics& ) = delete;
	Graphics& operator=( const Graphics& ) = delete;
	void EndFrame();
	void BeginFrame();
	Color GetPixel( int x,int y ) const;
	void PutPixel( int x,int y,int r,int g,int b )
	{
		PutPixel( x,y,{ unsigned char( r ),unsigned char( g ),unsigned char( b ) } );
	}
	void PutPixel( int x,int y,Color c );
	template<typename E>
	void DrawSprite( int x,int y,const Surface& s,E effect )
	{
		DrawSprite( x,y,s.GetRect(),s,effect );
	}
	template<typename E>
	void DrawSprite( int x,int y,const RectI& srcRect,const Surface& s,E effect )
	{
		DrawSprite( x,y,srcRect,GetScreenRect(),s,effect );
	}
	template<typename E>
	void DrawSprite( int x,int y,RectI srcRect,const RectI& clip,const Surface& s,E effect )
	{
		assert( srcRect.left >= 0 );
		assert( srcRect.right <= s.GetWidth() );
		assert( srcRect.top >= 0 );
		assert( srcRect.bottom <= s.GetHeight() );
		if( x < clip.left )
		{
			srcRect.left += clip.left - x;
			x = clip.left;
		}
		if( y < clip.top )
		{
			srcRect.top += clip.top - y;
			y = clip.top;
		}
		if( x + srcRect.GetWidth() > clip.right )
		{
			srcRect.right -= x + srcRect.GetWidth() - clip.right;
		}
		if( y + srcRect.GetHeight() > clip.bottom )
		{
			srcRect.bottom -= y + srcRect.GetHeight() - clip.bottom;
		}
		for( int sy = srcRect.top; sy < srcRect.bottom; sy++ )
		{
			for( int sx = srcRect.left; sx < srcRect.right; sx++ )
			{
				effect(
					s.GetPixel( sx,sy ),
					x + sx - srcRect.left,
					y + sy - srcRect.top,
					*this
				);
			}
		}
	}
	void DrawCircle(int x, int y, int radius, Color c);
	void DrawRect(int x0, int y0, int x1, int y1, Color c);
	void DrawRectDim(int x0, int y0, int width, int height, Color c)
	{
		DrawRect(x0, y0, x0 + width, y0 + height, c);
	}
	void DrawLine(int x0, int y0, int x1, int y1, Color c);
	void DrawLine(Vec2 pos0, Vec2 pos1, Color c)
	{
		DrawLine((int)pos0.x, (int)pos0.y, (int)pos1.x, (int)pos1.y, c);
	}
	void DrawLine(Vei2 pos0, Vei2 pos1, Color c)
	{
		DrawLine(pos0.x, pos0.y, pos1.x, pos1.y, c);
	}
	void DrawLines(std::vector<Vec2> points, Color c);
	void DrawShape(std::vector<Vec2> points, Color c);
	void FillBottomFlatTriangle(Vec2 v1, Vec2 v2, Vec2 v3, Color c);
	void FillTopFlatTriangle(Vec2 v1, Vec2 v2, Vec2 v3, Color c);
	void DrawTriangle(Vec2 p1, Vec2 p2, Vec2 p3, Color c);
	void DrawTriangle(Vei2 p1, Vei2 p2, Vei2 p3, Color c)
	{
		DrawTriangle(Vec2(p1), Vec2(p2), Vec2(p3), c);
	}
	struct sortYvalue {
		bool operator() (Vec2 pt1, Vec2 pt2) { return (pt1.y < pt2.y); }
	} sortYvalue;
	~Graphics();
private:
	Microsoft::WRL::ComPtr<IDXGISwapChain>				pSwapChain;
	Microsoft::WRL::ComPtr<ID3D11Device>				pDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>			pImmediateContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>		pRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D>				pSysBufferTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	pSysBufferTextureView;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>			pPixelShader;
	Microsoft::WRL::ComPtr<ID3D11VertexShader>			pVertexShader;
	Microsoft::WRL::ComPtr<ID3D11Buffer>				pVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>			pInputLayout;
	Microsoft::WRL::ComPtr<ID3D11SamplerState>			pSamplerState;
	D3D11_MAPPED_SUBRESOURCE							mappedSysBufferTexture;
	Color*                                              pSysBuffer = nullptr;
public:
	static constexpr int ScreenWidth = 800;
	static constexpr int ScreenHeight = 600;
	static RectI GetScreenRect();
};

#include "SpriteEffect.h"

#ifndef GOD_GRAPHICS
extern template
void Graphics::DrawSprite<SpriteEffect::Copy>( int x,int y,RectI srcRect,const RectI& clip,const Surface& s,SpriteEffect::Copy effect );
extern template
void Graphics::DrawSprite<SpriteEffect::Chroma>( int x,int y,RectI srcRect,const RectI& clip,const Surface& s,SpriteEffect::Chroma effect );
extern template
void Graphics::DrawSprite<SpriteEffect::Substitution>( int x,int y,RectI srcRect,const RectI& clip,const Surface& s,SpriteEffect::Substitution effect );
#endif