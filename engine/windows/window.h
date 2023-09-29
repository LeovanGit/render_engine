#pragma once

#include <windows.h>
#include <vector>
#include "glm/glm.hpp"

#include "../input/input_listener.h"
#include "../source/colors.h"

#include "../source/stdafx.h"

#include "../render/d3d.h"
#include "../render/engine.h"

void AttachListener(InputListener *input_listener);

namespace engine
{
class Window
{
public:
	Window(HINSTANCE app_handle,
		uint16_t pos_x,
		uint16_t pos_y,
		uint16_t client_width,
		uint16_t client_height,
		uint8_t stretch_factor = 1,
		float fps_limit = 60.0f);

	void AttachListener(InputListener *input_listener);

	const glm::u16vec2 &GetSize() const;
	const glm::u16vec2 &GetClientAreaSize() const;

	const glm::u16vec2 &GetPosition() const;

	std::vector<uint32_t> &GetPixelsBuffer();
	const glm::u16vec2 &GetPixelsBufferSize() const;

	float GetStretchFactor() const;

	void Move(uint16_t pos_x, uint16_t pos_y);
	void Resize(uint16_t width, uint16_t height);

	void Show(bool show = true);

	void Clear();

	void Flush();

	~Window();

private:
	void InitWindowClass(HINSTANCE app_handle);
	void Init(HINSTANCE app_handle);
	void InitPixelsBuffer();
	void CreateSwapChain(float fps_limit);

	// hack to make WindowProc class field:
	LRESULT CALLBACK WindowProc(HWND win_handle, UINT message, WPARAM w_param, LPARAM l_param);
	static LRESULT CALLBACK MessageRouter(HWND win_handle, UINT message, WPARAM w_param, LPARAM l_param);

	InputListener *m_input_listener;

	glm::u16vec2 m_position;
	glm::u16vec2 m_client_size;

	HWND m_handle;
	HDC m_hdc;
	WNDCLASSEX m_window_class;

	// for more performance in future
	// 1 pixels buffer pixel is stretchFactor screen pixels:
	uint8_t m_stretch_factor;

	BITMAPINFO m_bmp_info;
	std::vector<uint32_t> m_pixels;
	glm::u16vec2 m_pixels_size;

	wrl::ComPtr<IDXGISwapChain> m_swapchain;
};
} // namespace engine
