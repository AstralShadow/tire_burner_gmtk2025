#include "startup/startup.hpp"
#include "startup/logo.hpp"
#include "utils/fonts.hpp"
#include "core/core.hpp"
#include "utils/screen.hpp"
#include "utils/textures.hpp"
#include <SDL2/SDL_render.h>
#include <string>
#include <iostream>

using std::cout;
using std::endl;
using std::string;

static auto& rnd = core::renderer;


void startup::render(scene_uid)
{
    SDL_Color bg {0x05, 0x12, 0x1a, 255};
    SDL_SetRenderDrawColor(rnd, bg.r, bg.g, bg.b, bg.a);
    SDL_RenderClear(rnd);

    /* "Created for" text */
    if(time > text_enter_start)
    if(time < text_exit_end) {
        auto font = get_font(game::FT_DEFAULT, 40);
        SDL_Color color {255, 255, 255, 255};

        string text = "Created in 96h for:";
        auto length = text.size();
        auto first = 0;

        if(time < text_enter_end) {
            auto k = 1.0f * (time - text_enter_start)
                / (text_enter_end - text_enter_start);
            length *= k;
        } else if (time > text_exit_start) {
            auto k = 1 - 1.0f * (time - text_exit_start)
                / (text_exit_end - text_exit_start);
            length *= k;
            first = text.size() - length;
        }

        string subtext = text.substr(first, length);

        if(length > 0) {
            auto surf = TTF_RenderUTF8_Blended
                (font, subtext.c_str(), color);

            if(!surf) {
                cout << "Failed to render text" << endl;
                cout << TTF_GetError() << endl;
            } else {
                auto tex = utils::create_texture(surf);

                auto screen = screen_size();
                SDL_Rect dst {
                    (screen.x - surf->w) / 2,
                    64,
                    surf->w, surf->h
                };

                SDL_RenderCopy(rnd, tex, nullptr, &dst);

                SDL_DestroyTexture(tex);
                SDL_FreeSurface(surf);
            }
        }
    }

    /* Credits */
    {
        float k = 0.0f;

        if(time > creds_enter_start)
        if(time < creds_enter_end) {
            k = 1.0f * (time - creds_enter_start)
                / (creds_enter_end - creds_enter_start);
        }

        if(time >= creds_enter_end)
        if(time < creds_exit_start) {
            k = 1.0f;
        }

        if(time > creds_exit_start)
        if(time < creds_exit_end) {
            k = 1 - 1.0f * (time - creds_exit_start)
                / (creds_exit_end - creds_exit_start);
        }

        SDL_Color color {
            static_cast<u8>(bg.r + k * (255 - bg.r)),
            static_cast<u8>(bg.g + k * (255 - bg.g)),
            static_cast<u8>(bg.b + k * (255 - bg.b)),
            static_cast<u8>(bg.a + k * (255 - bg.a)),
        };


        SDL_Texture* texture = nullptr;
        SDL_Point size {0, 0};


        auto font = get_font(game::FT_DEFAULT);

        string text =
            "Art:  Jennifer Skot\n"
            "Code: AzCraft";

        auto surf = TTF_RenderUTF8_Blended_Wrapped
            (font, text.c_str(), color, 0);

        if(!surf) {
            cout << "Failed to render text" << endl;
            cout << TTF_GetError() << endl;
        } else {
            texture = utils::create_texture(surf);
            size = {surf->w, surf->h};
            SDL_FreeSurface(surf);
        }

        if(texture) {
            auto screen = screen_size();
            SDL_Rect dst {
                (screen.x - size.x) / 2,
                screen.y - size.y - 8,
                size.x, size.y
            };
            SDL_RenderCopy(rnd, texture, nullptr, &dst);
            SDL_DestroyTexture(texture);
        }
    }


    /* GMTK 2023 logo */
    auto logo = gmtk_logo();
    SDL_RenderCopy(rnd, logo, nullptr, &gmtk_logo_pos);


    SDL_RenderPresent(rnd);
}
