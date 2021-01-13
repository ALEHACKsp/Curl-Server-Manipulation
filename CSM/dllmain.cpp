/*
* MIT License
*
* Copyright (c) 2021 Eternal
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
*/


#include <Windows.h>
#include "CurlDefinitions.h"
#include "scanner.h"
#include "xorstr.hpp"
#include "MinHook.h"

//Original curl_easy_setopt routine which will be used to set up curl and continue normal execution
auto (*_curl_easy_setopt)(CURL* cURL, uintptr_t option, ...) -> CURLcode;

//This routine is used for setting up curl. we will be hijacking this to change the values.
auto Hijacked_curl_easy_setopt(CURL* cURL, uintptr_t option, PVOID data) -> CURLcode
{
    std::string _data;

    switch (option)
    {
    case CURLOPT_URL:
        //The application has called curl_easy_setopt with URL option to set up the URL which its going to read
        
        //modify the URL
        // e.x (https://www.google.com)
        return _curl_easy_setopt(cURL, option, E("https://website.domain/link"));
        break;
    case CURLOPT_NOPROXY:
        //The application has called curl_easy_setopt with no proxy option to set up the proxy

        //change the proxy to whatever host you want to redirect to
        // e.x (google.com)
        return _curl_easy_setopt(cURL, option, E("website.domain"));
        break;
    case CURLOPT_SSL_VERIFYPEER:
        //The application has called curl_easy_setopt with verify peer option to set up the peer check

        //change verify peer to 0 (off) or it will error
        return _curl_easy_setopt(cURL, option, 0);
        break;
    case CURLOPT_SSL_VERIFYHOST:
        //The application has called curl_easy_setopt with verify host option to set up the host check

        //change verify host to 0 (off) or it will error
        return _curl_easy_setopt(cURL, option, 0);
        break;
    case CURLOPT_PINNEDPUBLICKEY:
        //The application has called curl_easy_setopt with public key pinning option to set up ssl pinning

        //return ok because we want to disable ssl pinning so we can use custom urls
        return CURLcode::CURLE_OK;
        break;
    }

    //if the option is not of our interest, we will continue normal execution.
    return _curl_easy_setopt(cURL, option, data);
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {
        //initialize minhook
        auto status = MH_Initialize();
        if (status != MH_OK)
        {
            //minhook failed, error with what went wrong
            MessageBoxA(0, std::string(E("Failed To Init Hooking Library Error Code: ") + (std::string)MH_StatusToString(status)).c_str(), E("CSM Fatal Error!"), MB_ICONERROR);
            return FALSE;
        }

        //pattern scan the functions we're going to hook
        auto CurlEasyOpt_ = sigscan(E("89 54 24 10 4C 89 44 24 18 4C 89 4C 24 20 48 83 EC 28 48 85 C9"));

        if (!CurlEasyOpt_)
        {
            //Pattern Not Found
            MessageBoxA(0, E("Failed To Find curl_easy_setopt Function!"), E("CSM Fatal Error!"), MB_ICONERROR);
            return FALSE;
        }

        //create hook to the functions
        MH_CreateHook((void*)CurlEasyOpt_, Hijacked_curl_easy_setopt, (void**)&_curl_easy_setopt);

        //enable the hook
        status = MH_EnableHook((void*)CurlEasyOpt_);
        if (status != MH_OK)
        {
            //something went wrong, error and show what went wrong
            MessageBoxA(0, std::string(E("Failed To Hook Curl_Easy_SetOpt To Hijack, ERROR: ") + (std::string)MH_StatusToString(status)).c_str(), E("CSM Fatal Error!"), MB_ICONERROR);
            return FALSE;
        }

        //alert the user that CSM has successfully loaded
        MessageBoxA(0, E("Loaded CSM (cURL Server Manipulation)!"), E("CSM By Eternal#6666"), MB_ICONINFORMATION);
    }
    return TRUE;
}   