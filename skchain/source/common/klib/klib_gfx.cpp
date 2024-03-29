#include "klib_gfx.h"

// blitting
void klib::gfx::blit(SDL_Renderer* p_rnd, SDL_Texture* p_texture, int p_x, int p_y) {
	SDL_Rect t_rect{ p_x, p_y };
	SDL_QueryTexture(p_texture, nullptr, nullptr, &t_rect.w, &t_rect.h);

	SDL_RenderCopy(p_rnd, p_texture, nullptr, &t_rect);
}

void klib::gfx::blit_scale(SDL_Renderer* p_rnd, SDL_Texture* p_texture, int p_x, int p_y, int p_scale) {
	SDL_Rect t_rect{ p_x, p_y };
	SDL_QueryTexture(p_texture, nullptr, nullptr, &t_rect.w, &t_rect.h);
	t_rect.w *= p_scale;
	t_rect.h *= p_scale;

	SDL_RenderCopy(p_rnd, p_texture, nullptr, &t_rect);
}

void klib::gfx::blit_full_spec(SDL_Renderer* p_rnd, SDL_Texture* txt, int target_x, int target_y, int target_w, int target_h, int src_x, int src_y, int src_w, int src_h) {
	SDL_Rect src_rect{ src_x, src_y, src_w, src_h },
		target_rect{ target_x, target_y, target_w, target_h };

	SDL_RenderCopy(p_rnd, txt, &src_rect, &target_rect);
}

// drawing
void klib::gfx::draw_rect(SDL_Renderer* p_rnd, int p_x, int p_y, int p_w, int p_h, SDL_Color p_color, int p_thickness) {
	SDL_SetRenderDrawColor(p_rnd, p_color.r, p_color.g, p_color.b, p_color.a);

	for (int i = 0; i <= p_thickness; ++i) {
		SDL_Rect r{
		p_x + i,
		p_y + i,
		p_w - 2 * i,
		p_h - 2 * i };
		if (p_thickness == 0) {
			SDL_RenderFillRect(p_rnd, &r);
			return;
		}
		else
			SDL_RenderDrawRect(p_rnd, &r);
	}
}

std::vector<SDL_Texture*> klib::gfx::split_surface(SDL_Renderer* rnd, SDL_Surface* full_surface, int p_w, int p_h, bool p_destroy_surface, bool p_transparent, SDL_Color p_transp_col) {
	std::vector<SDL_Texture*> result;

	SDL_Rect tmpRectangle{ 0, 0, p_w, p_h };

	for (int j = 0; j < full_surface->h; j += p_h)
		for (int i = 0; i < full_surface->w; i += p_w) {
			tmpRectangle.x = i;
			tmpRectangle.y = j;

			SDL_Surface* tmp = SDL_CreateRGBSurface(0, p_w, p_h, 24, 0, 0, 0, 0);
			if (p_transparent)
				SDL_SetColorKey(tmp, true, SDL_MapRGB(tmp->format, p_transp_col.r, p_transp_col.g, p_transp_col.b));
			SDL_BlitSurface(full_surface, &tmpRectangle, tmp, nullptr);
			result.push_back(surface_to_texture(rnd, tmp, p_destroy_surface));
		}

	return(result);
}

// extracts textures from a given surface
// all textures to extract are specified in the vector of SDL_Rects
std::vector<SDL_Texture*> klib::gfx::split_surface_specified(SDL_Renderer* p_rnd, SDL_Surface* srf,
	const std::vector<SDL_Rect>& p_rects, bool p_destroy_surface, bool p_transparent, SDL_Color p_transp_col) {
	std::vector<SDL_Texture*> result;

	for (const auto& rect : p_rects) {
		SDL_Surface* tmp = SDL_CreateRGBSurface(0, rect.w, rect.h, 24, 0, 0, 0, 0);
		if (p_transparent)
			SDL_SetColorKey(tmp, true, SDL_MapRGB(tmp->format, p_transp_col.r, p_transp_col.g, p_transp_col.b));
		SDL_BlitSurface(srf, &rect, tmp, nullptr);
		result.push_back(surface_to_texture(p_rnd, tmp, p_destroy_surface));
	}

	return result;
}

SDL_Texture* klib::gfx::surface_to_texture(SDL_Renderer* p_rnd, SDL_Surface* p_srf, bool p_destroy_surface) {
	SDL_Texture* result = SDL_CreateTextureFromSurface(p_rnd, p_srf);
	if (p_destroy_surface)
		SDL_FreeSurface(p_srf);
	return(result);
}

void klib::gfx::put_pixel(SDL_Surface* srf, int x, int y, Uint32 pixel) {
	SDL_LockSurface(srf);

	int bpp = srf->format->BytesPerPixel;
	/* Here p is the address to the pixel we want to set */
	Uint8* p = (Uint8*)srf->pixels + y * srf->pitch + x * bpp;

	switch (bpp) {
	case 1:
		*p = pixel;
		break;

	case 2:
		*(Uint16*)p = pixel;
		break;

	case 3:
		if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
			p[0] = (pixel >> 16) & 0xff;
			p[1] = (pixel >> 8) & 0xff;
			p[2] = pixel & 0xff;
		}
		else {
			p[0] = pixel & 0xff;
			p[1] = (pixel >> 8) & 0xff;
			p[2] = (pixel >> 16) & 0xff;
		}
		break;

	case 4:
		*(Uint32*)p = pixel;
		break;

	default:
		break;           /* shouldn't happen, but avoids warnings */
	}

	SDL_UnlockSurface(srf);
}

Uint32 klib::gfx::get_pixel(SDL_Surface* surface, int x, int y) {
	int bpp = surface->format->BytesPerPixel;
	/* Here p is the address to the pixel we want to retrieve */
	Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;

	switch (bpp)
	{
	case 1:
		return *p;
		break;

	case 2:
		return *(Uint16*)p;
		break;

	case 3:
		if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
			return p[0] << 16 | p[1] << 8 | p[2];
		else
			return p[0] | p[1] << 8 | p[2] << 16;
		break;

	case 4:
		return *(Uint32*)p;
		break;

	default:
		return 0;       /* shouldn't happen, but avoids warnings */
	}
}

SDL_Color klib::gfx::pulse_color(SDL_Color a, SDL_Color b, int p_progress) {
	float l_progress{ static_cast<float>(p_progress / 255.0f) };

	Uint8 cr = static_cast<Uint8>(a.r + (b.r - a.r) * l_progress);
	Uint8 cg = static_cast<Uint8>(a.g + (b.g - a.g) * l_progress);
	Uint8 cb = static_cast<Uint8>(a.b + (b.b - a.b) * l_progress);
	Uint8 ca = static_cast<Uint8>(a.a + (b.a - a.a) * l_progress);
	return SDL_Color{ cr,cg,cb,ca };
}
