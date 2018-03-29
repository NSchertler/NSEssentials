#pragma once

#ifdef NSE_BUILD_SHARED
#ifndef NSE_EXPORT
#    ifdef _WIN32
#      ifdef nsessentials_EXPORTS
         /* We are building this library */
#        define NSE_EXPORT __declspec(dllexport)
#      else
         /* We are using this library */
#        define NSE_EXPORT __declspec(dllimport)
#      endif
#    else
#      define NSE_EXPORT
#    endif
#  endif
#else
#  define NSE_EXPORT
#endif