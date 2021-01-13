# Curl Server Manipulation (CSM)

This tool can be used to manipulate setup data for curl to configure settings like public key for ssl public key pinning, URL, post data and many more.

## How does this work?

This tool hooks a function called curl_easy_setopt, once the function is hooked and then any calls to it will be redirected to our own function where we can decide what to do with the parameters and whether we should send a call to the function, for example in the target application we're setting the url to https://www.google.com which calls the curl_easy_setopt function which calls our own function since we put a jump to our own function in the first instruction in the function. Then we can modify the data by checking the option then we call our own code specified with the option so we're changing the url to https://www.speedtest.net. 


## How can this be detected?

The hook can be detected in many ways, for example utilizing crc and comparing the hash later on, or checking the first couple of bytes to check if its a jmp instruction. Keep in mind this is only a POC.

## How do I utilize this?

This tool is very easy to utilize as you only need to build it in (x64 release) and in visual studio, make sure the SDK's are installed! Then we can move to the second step which is basically injecting our dll library to the target application, please remember to change the parameters in the source to your likings otherwise it **WILL NOT** work as you suspected.


## HELP IM STUCK AAAAA!!!!!

Please don't DM me on discord with really stupid questions because I will block you.


## HELP I'm getting MH_ERROR_MEMORY_PROTECT error when injecting!

This issue is due to VMP (VMProtect) (mostly) because of the nature of VMProtect and it overwriting the sections, if your target application is not protected with VMProtect then there might be another issue, maybe restart your target application. If the issue still persists then use https://docs.microsoft.com/en-us/windows/win32/debug/retrieving-the-last-error-code to get more information regarding what went wrong.


## HELP I'm getting "Failed To Find curl_easy_setopt Function!" error!!

The pattern is outdated.




## Credits

Main project: Me (Eternal#9999 / furiosdestruct)

CURL: Daniel Stenberg | https://github.com/curl/curl

MinHook: Tsuda Kageyu | https://github.com/TsudaKageyu/minhook

XorStr: JustasMasiulis | https://github.com/JustasMasiulis/xorstr
