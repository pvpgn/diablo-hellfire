/*==========================================================================;
 *
 *  Copyright (C) 1995,1996 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       dsound.h
 *  Content:    DirectSound include file
 *
 ***************************************************************************/

#ifndef __DSOUND_INCLUDED__
#define __DSOUND_INCLUDED__

#include "d3dtypes.h"

#ifdef _WIN32
#define COM_NO_WINDOWS_H
#include <objbase.h>
#endif

#define _FACDS  0x878
#define MAKE_DSHRESULT( code )  MAKE_HRESULT( 1, _FACDS, code )

#ifdef __cplusplus
extern "C" {
#endif

// Direct Sound Component GUID    {47D4D946-62E8-11cf-93BC-444553540000}
DEFINE_GUID(CLSID_DirectSound,
0x47d4d946, 0x62e8, 0x11cf, 0x93, 0xbc, 0x44, 0x45, 0x53, 0x54, 0x0, 0x0);

// DirectSound 279afa83-4981-11ce-a521-0020af0be560
DEFINE_GUID(IID_IDirectSound,0x279AFA83,0x4981,0x11CE,0xA5,0x21,0x00,0x20,0xAF,0x0B,0xE5,0x60);
// DirectSoundBuffer 279afa85-4981-11ce-a521-0020af0be560
DEFINE_GUID(IID_IDirectSoundBuffer,0x279AFA85,0x4981,0x11CE,0xA5,0x21,0x00,0x20,0xAF,0x0B,0xE5,0x60);

//DirectSound3DListener 279afa84-4981-11ce-a521-0020af0be560
DEFINE_GUID(IID_IDirectSound3DListener,0x279AFA84,0x4981,0x11CE,0xA5,0x21,0x00,0x20,0xAF,0x0B,0xE5,0x60);
//DirectSound3DBuffer 279afa86-4981-11ce-a521-0020af0be560
DEFINE_GUID(IID_IDirectSound3DBuffer,0x279AFA86,0x4981,0x11CE,0xA5,0x21,0x00,0x20,0xAF,0x0B,0xE5,0x60);


//==========================================================================;
//
//                            Structures...
//
//==========================================================================;
#ifdef __cplusplus
/* 'struct' not 'class' per the way DECLARE_INTERFACE_ is defined */
struct IDirectSound;
struct IDirectSoundBuffer;
struct IDirectSound3DListener;
struct IDirectSound3DBuffer;
#endif

typedef struct IDirectSound           *LPDIRECTSOUND;        
typedef struct IDirectSoundBuffer     *LPDIRECTSOUNDBUFFER;  
typedef struct IDirectSoundBuffer    **LPLPDIRECTSOUNDBUFFER;  
typedef struct IDirectSound3DListener         *LPDIRECTSOUND3DLISTENER;
typedef struct IDirectSound3DBuffer   *LPDIRECTSOUND3DBUFFER;


typedef struct _DSCAPS
{
    DWORD       dwSize;
    DWORD       dwFlags;
    DWORD       dwMinSecondarySampleRate;
    DWORD       dwMaxSecondarySampleRate;
    DWORD       dwPrimaryBuffers;
    DWORD       dwMaxHwMixingAllBuffers;
    DWORD       dwMaxHwMixingStaticBuffers;
    DWORD       dwMaxHwMixingStreamingBuffers;
    DWORD       dwFreeHwMixingAllBuffers;
    DWORD       dwFreeHwMixingStaticBuffers;
    DWORD       dwFreeHwMixingStreamingBuffers;
    DWORD       dwMaxHw3DAllBuffers;
    DWORD       dwMaxHw3DStaticBuffers;
    DWORD       dwMaxHw3DStreamingBuffers;
    DWORD       dwFreeHw3DAllBuffers;
    DWORD       dwFreeHw3DStaticBuffers;
    DWORD       dwFreeHw3DStreamingBuffers;
    DWORD       dwTotalHwMemBytes;
    DWORD       dwFreeHwMemBytes;
    DWORD       dwMaxContigFreeHwMemBytes;
    DWORD       dwUnlockTransferRateHwBuffers;
    DWORD       dwPlayCpuOverheadSwBuffers;
    DWORD       dwReserved1;
    DWORD       dwReserved2;
} DSCAPS, *LPDSCAPS;

typedef struct _DSBCAPS
{
    
    DWORD       dwSize;
    DWORD       dwFlags;
    DWORD       dwBufferBytes;
    DWORD       dwUnlockTransferRate;
    DWORD       dwPlayCpuOverhead;
} DSBCAPS, *LPDSBCAPS;

typedef struct _DSBUFFERDESC
{
    DWORD                   dwSize;
    DWORD                   dwFlags;
    DWORD                   dwBufferBytes;
    DWORD                   dwReserved;
    LPWAVEFORMATEX          lpwfxFormat;
} DSBUFFERDESC, *LPDSBUFFERDESC;

typedef struct _DS3DBUFFER
{
    DWORD       dwSize;
    D3DVECTOR   vPosition;
    D3DVECTOR   vVelocity;
    DWORD       dwInsideConeAngle;
    DWORD       dwOutsideConeAngle;
    D3DVECTOR   vConeOrientation;
    LONG        lConeOutsideVolume;
    D3DVALUE    flMinDistance;
    D3DVALUE    flMaxDistance;
    DWORD       dwMode;
} DS3DBUFFER, *LPDS3DBUFFER;

typedef struct _DS3DLISTENER
{
    DWORD       dwSize;
    D3DVECTOR   vPosition;
    D3DVECTOR   vVelocity;
    D3DVECTOR   vOrientFront;
    D3DVECTOR   vOrientTop;
    D3DVALUE    flDistanceFactor;
    D3DVALUE    flRolloffFactor;
    D3DVALUE    flDopplerFactor;
} DS3DLISTENER, *LPDS3DLISTENER;



typedef LPVOID* LPLPVOID;


typedef BOOL (FAR PASCAL * LPDSENUMCALLBACKW)(const GUID FAR *, LPWSTR, LPWSTR, LPVOID);
typedef BOOL (FAR PASCAL * LPDSENUMCALLBACKA)(const GUID FAR *, LPSTR, LPSTR, LPVOID);

extern HRESULT WINAPI DirectSoundCreate(const GUID * lpGUID, LPDIRECTSOUND * ppDS, IUnknown FAR *pUnkOuter );
extern HRESULT WINAPI DirectSoundEnumerateW(LPDSENUMCALLBACKW lpCallback, LPVOID lpContext );
extern HRESULT WINAPI DirectSoundEnumerateA(LPDSENUMCALLBACKA lpCallback, LPVOID lpContext );

#ifdef UNICODE
#define LPDSENUMCALLBACK        LPDSENUMCALLBACKW
#define DirectSoundEnumerate    DirectSoundEnumerateW
#else
#define LPDSENUMCALLBACK        LPDSENUMCALLBACKA
#define DirectSoundEnumerate    DirectSoundEnumerateA
#endif

//
// IDirectSound
//
#undef INTERFACE
#define INTERFACE IDirectSound
#ifdef _WIN32
DECLARE_INTERFACE_( IDirectSound, IUnknown )
{
    /*** IUnknown methods ***/
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID * ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;
    /*** IDirectSound methods ***/

    STDMETHOD( CreateSoundBuffer)(THIS_ LPDSBUFFERDESC, LPLPDIRECTSOUNDBUFFER, IUnknown FAR *) PURE;
    STDMETHOD( GetCaps)(THIS_ LPDSCAPS ) PURE;
    STDMETHOD( DuplicateSoundBuffer)(THIS_ LPDIRECTSOUNDBUFFER, LPLPDIRECTSOUNDBUFFER ) PURE;
    STDMETHOD( SetCooperativeLevel)(THIS_ HWND, DWORD ) PURE;
    STDMETHOD( Compact)(THIS ) PURE;
    STDMETHOD( GetSpeakerConfig)(THIS_ LPDWORD ) PURE;
    STDMETHOD( SetSpeakerConfig)(THIS_ DWORD ) PURE;
    STDMETHOD( Initialize)(THIS_ const GUID * ) PURE;
};

#if !defined(__cplusplus) || defined(CINTERFACE)
#define IDirectSound_QueryInterface(p,a,b)       (p)->lpVtbl->QueryInterface(p,a,b)
#define IDirectSound_AddRef(p)                   (p)->lpVtbl->AddRef(p)
#define IDirectSound_Release(p)                  (p)->lpVtbl->Release(p)
#define IDirectSound_CreateSoundBuffer(p,a,b,c)  (p)->lpVtbl->CreateSoundBuffer(p,a,b,c)
#define IDirectSound_GetCaps(p,a)                (p)->lpVtbl->GetCaps(p,a)
#define IDirectSound_DuplicateSoundBuffer(p,a,b) (p)->lpVtbl->DuplicateSoundBuffer(p,a,b)
#define IDirectSound_SetCooperativeLevel(p,a,b)  (p)->lpVtbl->SetCooperativeLevel(p,a,b)
#define IDirectSound_Compact(p)                  (p)->lpVtbl->Compact(p)
#define IDirectSound_GetSpeakerConfig(p,a)       (p)->lpVtbl->GetSpeakerConfig(p,a)
#define IDirectSound_SetSpeakerConfig(p,b)       (p)->lpVtbl->SetSpeakerConfig(p,b)
#define IDirectSound_Initialize(p,a)             (p)->lpVtbl->Initialize(p,a)
#else
#define IDirectSound_QueryInterface(p,a,b)       (p)->QueryInterface(a,b)
#define IDirectSound_AddRef(p)                   (p)->AddRef()
#define IDirectSound_Release(p)                  (p)->Release()
#define IDirectSound_CreateSoundBuffer(p,a,b,c)  (p)->CreateSoundBuffer(a,b,c)
#define IDirectSound_GetCaps(p,a)                (p)->GetCaps(a)
#define IDirectSound_DuplicateSoundBuffer(p,a,b) (p)->DuplicateSoundBuffer(a,b)
#define IDirectSound_SetCooperativeLevel(p,a,b)  (p)->SetCooperativeLevel(a,b)
#define IDirectSound_Compact(p)                  (p)->Compact()
#define IDirectSound_GetSpeakerConfig(p,a)       (p)->GetSpeakerConfig(a)
#define IDirectSound_SetSpeakerConfig(p,b)       (p)->SetSpeakerConfig(b)
#define IDirectSound_Initialize(p,a)             (p)->Initialize(a)
#endif

#endif

//
// IDirectSoundBuffer
//
#undef INTERFACE
#define INTERFACE IDirectSoundBuffer
#ifdef _WIN32
DECLARE_INTERFACE_( IDirectSoundBuffer, IUnknown )
{
    /*** IUnknown methods ***/
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID * ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;
    /*** IDirectSoundBuffer methods ***/

    STDMETHOD(           GetCaps)(THIS_ LPDSBCAPS ) PURE;
    STDMETHOD(GetCurrentPosition)(THIS_ LPDWORD,LPDWORD ) PURE;
    STDMETHOD(         GetFormat)(THIS_ LPWAVEFORMATEX, DWORD, LPDWORD ) PURE;
    STDMETHOD(         GetVolume)(THIS_ LPLONG ) PURE;
    STDMETHOD(            GetPan)(THIS_ LPLONG ) PURE;
    STDMETHOD(      GetFrequency)(THIS_ LPDWORD ) PURE;
    STDMETHOD(         GetStatus)(THIS_ LPDWORD ) PURE;
    STDMETHOD(        Initialize)(THIS_ LPDIRECTSOUND, LPDSBUFFERDESC ) PURE;
    STDMETHOD(              Lock)(THIS_ DWORD,DWORD,LPVOID,LPDWORD,LPVOID,LPDWORD,DWORD ) PURE;
    STDMETHOD(              Play)(THIS_ DWORD,DWORD,DWORD ) PURE;
    STDMETHOD(SetCurrentPosition)(THIS_ DWORD ) PURE;
    STDMETHOD(         SetFormat)(THIS_ LPWAVEFORMATEX ) PURE;
    STDMETHOD(         SetVolume)(THIS_ LONG ) PURE;
    STDMETHOD(            SetPan)(THIS_ LONG ) PURE;
    STDMETHOD(      SetFrequency)(THIS_ DWORD ) PURE;
    STDMETHOD(              Stop)(THIS  ) PURE;
    STDMETHOD(            Unlock)(THIS_ LPVOID,DWORD,LPVOID,DWORD ) PURE;
    STDMETHOD(           Restore)(THIS  ) PURE;
};

#if !defined(__cplusplus) || defined(CINTERFACE)
#define IDirectSoundBuffer_QueryInterface(p,a,b)        (p)->lpVtbl->QueryInterface(p,a,b)
#define IDirectSoundBuffer_AddRef(p)                    (p)->lpVtbl->AddRef(p)
#define IDirectSoundBuffer_Release(p)                   (p)->lpVtbl->Release(p)
#define IDirectSoundBuffer_GetCaps(p,a)                 (p)->lpVtbl->GetCaps(p,a)
#define IDirectSoundBuffer_GetCurrentPosition(p,a,b)    (p)->lpVtbl->GetCurrentPosition(p,a,b)
#define IDirectSoundBuffer_GetFormat(p,a,b,c)           (p)->lpVtbl->GetFormat(p,a,b,c)
#define IDirectSoundBuffer_GetVolume(p,a)               (p)->lpVtbl->GetVolume(p,a)
#define IDirectSoundBuffer_GetPan(p,a)                  (p)->lpVtbl->GetPan(p,a)
#define IDirectSoundBuffer_GetFrequency(p,a)            (p)->lpVtbl->GetFrequency(p,a)
#define IDirectSoundBuffer_GetStatus(p,a)               (p)->lpVtbl->GetStatus(p,a)
#define IDirectSoundBuffer_Initialize(p,a,b)            (p)->lpVtbl->Initialize(p,a,b)
#define IDirectSoundBuffer_Lock(p,a,b,c,d,e,f,g)        (p)->lpVtbl->Lock(p,a,b,c,d,e,f,g)
#define IDirectSoundBuffer_Play(p,a,b,c)                (p)->lpVtbl->Play(p,a,b,c)
#define IDirectSoundBuffer_SetCurrentPosition(p,a)      (p)->lpVtbl->SetCurrentPosition(p,a)
#define IDirectSoundBuffer_SetFormat(p,a)               (p)->lpVtbl->SetFormat(p,a)
#define IDirectSoundBuffer_SetVolume(p,a)               (p)->lpVtbl->SetVolume(p,a)
#define IDirectSoundBuffer_SetPan(p,a)                  (p)->lpVtbl->SetPan(p,a)
#define IDirectSoundBuffer_SetFrequency(p,a)            (p)->lpVtbl->SetFrequency(p,a)
#define IDirectSoundBuffer_Stop(p)                      (p)->lpVtbl->Stop(p)
#define IDirectSoundBuffer_Unlock(p,a,b,c,d)            (p)->lpVtbl->Unlock(p,a,b,c,d)
#define IDirectSoundBuffer_Restore(p)                   (p)->lpVtbl->Restore(p)
#else
#define IDirectSoundBuffer_QueryInterface(p,a,b)        (p)->QueryInterface(a,b)
#define IDirectSoundBuffer_AddRef(p)                    (p)->AddRef()
#define IDirectSoundBuffer_Release(p)                   (p)->Release()
#define IDirectSoundBuffer_GetCaps(p,a)                 (p)->GetCaps(a)
#define IDirectSoundBuffer_GetCurrentPosition(p,a,b)    (p)->GetCurrentPosition(a,b)
#define IDirectSoundBuffer_GetFormat(p,a,b,c)           (p)->GetFormat(a,b,c)
#define IDirectSoundBuffer_GetVolume(p,a)               (p)->GetVolume(a)
#define IDirectSoundBuffer_GetPan(p,a)                  (p)->GetPan(a)
#define IDirectSoundBuffer_GetFrequency(p,a)            (p)->GetFrequency(a)
#define IDirectSoundBuffer_GetStatus(p,a)               (p)->GetStatus(a)
#define IDirectSoundBuffer_Initialize(p,a,b)            (p)->Initialize(a,b)
#define IDirectSoundBuffer_Lock(p,a,b,c,d,e,f,g)        (p)->Lock(a,b,c,d,e,f,g)
#define IDirectSoundBuffer_Play(p,a,b,c)                (p)->Play(a,b,c)
#define IDirectSoundBuffer_SetCurrentPosition(p,a)      (p)->SetCurrentPosition(a)
#define IDirectSoundBuffer_SetFormat(p,a)               (p)->SetFormat(a)
#define IDirectSoundBuffer_SetVolume(p,a)               (p)->SetVolume(a)
#define IDirectSoundBuffer_SetPan(p,a)                  (p)->SetPan(a)
#define IDirectSoundBuffer_SetFrequency(p,a)            (p)->SetFrequency(a)
#define IDirectSoundBuffer_Stop(p)                      (p)->Stop()
#define IDirectSoundBuffer_Unlock(p,a,b,c,d)            (p)->Unlock(a,b,c,d)
#define IDirectSoundBuffer_Restore(p)                   (p)->Restore()
#endif

#endif

//
// IDirectSound3DListener
//
#undef INTERFACE
#define INTERFACE IDirectSound3DListener
#ifdef _WIN32
DECLARE_INTERFACE_(IDirectSound3DListener, IUnknown)
{
    /*** IUnknown methods ***/
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID * ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;

    /*** IDirectSound3D methods ***/
    STDMETHOD(GetAllParameters)(THIS_ LPDS3DLISTENER) PURE;
    STDMETHOD(GetDistanceFactor)(THIS_ LPD3DVALUE) PURE;
    STDMETHOD(GetDopplerFactor)(THIS_ LPD3DVALUE) PURE;
    STDMETHOD(GetOrientation)(THIS_ LPD3DVECTOR, LPD3DVECTOR) PURE;
    STDMETHOD(GetPosition)(THIS_ LPD3DVECTOR) PURE;
    STDMETHOD(GetRolloffFactor)(THIS_ LPD3DVALUE ) PURE;
    STDMETHOD(GetVelocity)(THIS_ LPD3DVECTOR) PURE;
    STDMETHOD(SetAllParameters)(THIS_ LPDS3DLISTENER, DWORD) PURE;
    STDMETHOD(SetDistanceFactor)(THIS_ D3DVALUE, DWORD) PURE;
    STDMETHOD(SetDopplerFactor)(THIS_ D3DVALUE, DWORD) PURE;
    STDMETHOD(SetOrientation)(THIS_ D3DVALUE, D3DVALUE, D3DVALUE, D3DVALUE, D3DVALUE, D3DVALUE, DWORD) PURE;
    STDMETHOD(SetPosition)(THIS_ D3DVALUE, D3DVALUE, D3DVALUE, DWORD) PURE;
    STDMETHOD(SetRolloffFactor)(THIS_ D3DVALUE, DWORD) PURE;
    STDMETHOD(SetVelocity)(THIS_ D3DVALUE, D3DVALUE, D3DVALUE, DWORD) PURE;
    STDMETHOD(CommitDeferredSettings)(THIS) PURE;
};

#if !defined(__cplusplus) || defined(CINTERFACE)
#define IDirectSound3DListener_QueryInterface(p,a,b)            (p)->lpVtbl->QueryInterface(p,a,b)
#define IDirectSound3DListener_AddRef(p)                        (p)->lpVtbl->AddRef(p)
#define IDirectSound3DListener_Release(p)                       (p)->lpVtbl->Release(p)
#define IDirectSound3DListener_GetAllParameters(p,a)            (p)->lpVtbl->GetAllParameters(p,a)
#define IDirectSound3DListener_GetDistanceFactor(p,a)           (p)->lpVtbl->GetDistanceFactor(p,a)
#define IDirectSound3DListener_GetDopplerFactor(p,a)            (p)->lpVtbl->GetDopplerFactor(p,a)
#define IDirectSound3DListener_GetOrientation(p,a,b)            (p)->lpVtbl->GetOrientation(p,a,b)
#define IDirectSound3DListener_GetPosition(p,a)                 (p)->lpVtbl->GetPosition(p,a)
#define IDirectSound3DListener_GetRolloffFactor(p,a)            (p)->lpVtbl->GetRolloffFactor(p,a)
#define IDirectSound3DListener_GetVelocity(p,a)                 (p)->lpVtbl->GetVelocity(p,a)
#define IDirectSound3DListener_SetAllParameters(p,a,b)          (p)->lpVtbl->SetAllParameters(p,a,b)
#define IDirectSound3DListener_SetDistanceFactor(p,a,b)         (p)->lpVtbl->SetDistanceFactor(p,a,b)
#define IDirectSound3DListener_SetDopplerFactor(p,a,b)          (p)->lpVtbl->SetDopplerFactor(p,a,b)
#define IDirectSound3DListener_SetOrientation(p,a,b,c,d,e,f,g)  (p)->lpVtbl->SetOrientation(p,a,b,c,d,e,f,g)
#define IDirectSound3DListener_SetPosition(p,a,b,c,d)           (p)->lpVtbl->SetPosition(p,a,b,c,d)
#define IDirectSound3DListener_SetRolloffFactor(p,a,b)          (p)->lpVtbl->SetRolloffFactor(p,a,b)
#define IDirectSound3DListener_SetVelocity(p,a,b,c,d)           (p)->lpVtbl->SetVelocity(p,a,b,c,d)
#define IDirectSound3DListener_CommitDeferredSettings(p)        (p)->lpVtbl->CommitDeferredSettings(p)
#else
#define IDirectSound3DListener_QueryInterface(p,a,b)            (p)->QueryInterface(a,b)
#define IDirectSound3DListener_AddRef(p)                        (p)->AddRef()
#define IDirectSound3DListener_Release(p)                       (p)->Release()
#define IDirectSound3DListener_GetAllParameters(p,a)            (p)->GetAllParameters(a)
#define IDirectSound3DListener_GetDistanceFactor(p,a)           (p)->GetDistanceFactor(a)
#define IDirectSound3DListener_GetDopplerFactor(p,a)            (p)->GetDopplerFactor(a)
#define IDirectSound3DListener_GetOrientation(p,a,b)            (p)->GetOrientation(a,b)
#define IDirectSound3DListener_GetPosition(p,a)                 (p)->GetPosition(a)
#define IDirectSound3DListener_GetRolloffFactor(p,a)            (p)->GetRolloffFactor(a)
#define IDirectSound3DListener_GetVelocity(p,a)                 (p)->GetVelocity(a)
#define IDirectSound3DListener_SetAllParameters(p,a,b)          (p)->SetAllParameters(a,b)
#define IDirectSound3DListener_SetDistanceFactor(p,a,b)         (p)->SetDistanceFactor(a,b)
#define IDirectSound3DListener_SetDopplerFactor(p,a,b)          (p)->SetDopplerFactor(a,b)
#define IDirectSound3DListener_SetOrientation(p,a,b,c,d,e,f,g)  (p)->SetOrientation(a,b,c,d,e,f,g)
#define IDirectSound3DListener_SetPosition(p,a,b,c,d)           (p)->SetPosition(a,b,c,d)
#define IDirectSound3DListener_SetRolloffFactor(p,a,b)          (p)->SetRolloffFactor(a,b)
#define IDirectSound3DListener_SetVelocity(p,a,b,c,d)           (p)->SetVelocity(a,b,c,d)
#define IDirectSound3DListener_CommitDeferredSettings(p)        (p)->CommitDeferredSettings()
#endif

#endif

//
// IDirectSound3DBuffer
//
#undef INTERFACE
#define INTERFACE IDirectSound3DBuffer
#ifdef _WIN32
DECLARE_INTERFACE_(IDirectSound3DBuffer, IUnknown)
{
    /*** IUnknown methods ***/
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID * ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;

    /*** IDirectSoundBuffer3D methods ***/
    STDMETHOD(GetAllParameters)(THIS_ LPDS3DBUFFER) PURE;
    STDMETHOD(GetConeAngles)(THIS_ LPDWORD, LPDWORD) PURE;
    STDMETHOD(GetConeOrientation)(THIS_ LPD3DVECTOR) PURE;
    STDMETHOD(GetConeOutsideVolume)(THIS_ LPLONG) PURE;
    STDMETHOD(GetMaxDistance)(THIS_ LPD3DVALUE) PURE;
    STDMETHOD(GetMinDistance)(THIS_ LPD3DVALUE) PURE;
    STDMETHOD(GetMode)(THIS_ LPDWORD) PURE;
    STDMETHOD(GetPosition)(THIS_ LPD3DVECTOR) PURE;
    STDMETHOD(GetVelocity)(THIS_ LPD3DVECTOR) PURE;
    STDMETHOD(SetAllParameters)(THIS_ LPDS3DBUFFER, DWORD) PURE;
    STDMETHOD(SetConeAngles)(THIS_ DWORD, DWORD, DWORD) PURE;
    STDMETHOD(SetConeOrientation)(THIS_ D3DVALUE, D3DVALUE, D3DVALUE, DWORD) PURE;
    STDMETHOD(SetConeOutsideVolume)(THIS_ LONG, DWORD) PURE;
    STDMETHOD(SetMaxDistance)(THIS_ D3DVALUE, DWORD) PURE;
    STDMETHOD(SetMinDistance)(THIS_ D3DVALUE, DWORD) PURE;
    STDMETHOD(SetMode)(THIS_ DWORD, DWORD) PURE;
    STDMETHOD(SetPosition)(THIS_ D3DVALUE, D3DVALUE, D3DVALUE, DWORD) PURE;
    STDMETHOD(SetVelocity)(THIS_ D3DVALUE, D3DVALUE, D3DVALUE, DWORD) PURE;
};

#if !defined(__cplusplus) || defined(CINTERFACE)
#define IDirectSound3DBuffer_QueryInterface(p,a,b)      (p)->lpVtbl->QueryInterface(p,a,b)
#define IDirectSound3DBuffer_AddRef(p)                  (p)->lpVtbl->AddRef(p)
#define IDirectSound3DBuffer_Release(p)                 (p)->lpVtbl->Release(p)
#define IDirectSound3DBuffer_GetAllParameters(p,a)      (p)->lpVtbl->GetAllParameters(p,a)
#define IDirectSound3DBuffer_GetConeAngles(p,a,b)       (p)->lpVtbl->GetConeAngles(p,a,b)
#define IDirectSound3DBuffer_GetConeOrientation(p,a)    (p)->lpVtbl->GetConeOrientation(p,a)
#define IDirectSound3DBuffer_GetConeOutsideVolume(p,a)  (p)->lpVtbl->GetConeOutsideVolume(p,a)
#define IDirectSound3DBuffer_GetPosition(p,a)           (p)->lpVtbl->GetPosition(p,a)
#define IDirectSound3DBuffer_GetMinDistance(p,a)        (p)->lpVtbl->GetMinDistance(p,a)
#define IDirectSound3DBuffer_GetMaxDistance(p,a)        (p)->lpVtbl->GetMaxDistance(p,a)
#define IDirectSound3DBuffer_GetMode(p,a)               (p)->lpVtbl->GetMode(p,a)
#define IDirectSound3DBuffer_GetVelocity(p,a)           (p)->lpVtbl->GetVelocity(p,a)
#define IDirectSound3DBuffer_SetAllParameters(p,a,b)    (p)->lpVtbl->SetAllParameters(p,a,b)
#define IDirectSound3DBuffer_SetConeAngles(p,a,b,c)     (p)->lpVtbl->SetConeAngles(p,a,b,c)
#define IDirectSound3DBuffer_SetConeOrientation(p,a,b,c,d) (p)->lpVtbl->SetConeOrientation(p,a,b,c,d)
#define IDirectSound3DBuffer_SetConeOutsideVolume(p,a,b)(p)->lpVtbl->SetConeOutsideVolume(p,a,b)
#define IDirectSound3DBuffer_SetPosition(p,a,b,c,d)     (p)->lpVtbl->SetPosition(p,a,b,c,d)
#define IDirectSound3DBuffer_SetMinDistance(p,a,b)      (p)->lpVtbl->SetMinDistance(p,a,b)
#define IDirectSound3DBuffer_SetMaxDistance(p,a,b)      (p)->lpVtbl->SetMaxDistance(p,a,b)
#define IDirectSound3DBuffer_SetMode(p,a,b)             (p)->lpVtbl->SetMode(p,a,b)
#define IDirectSound3DBuffer_SetVelocity(p,a,b,c,d)     (p)->lpVtbl->SetVelocity(p,a,b,c,d)
#else
#define IDirectSound3DBuffer_QueryInterface(p,a,b)      (p)->QueryInterface(a,b)
#define IDirectSound3DBuffer_AddRef(p)                  (p)->AddRef()
#define IDirectSound3DBuffer_Release(p)                 (p)->Release()
#define IDirectSound3DBuffer_GetAllParameters(p,a)      (p)->GetAllParameters(a)
#define IDirectSound3DBuffer_GetConeAngles(p,a,b)       (p)->GetConeAngles(a,b)
#define IDirectSound3DBuffer_GetConeOrientation(p,a)    (p)->GetConeOrientation(a)
#define IDirectSound3DBuffer_GetConeOutsideVolume(p,a)  (p)->GetConeOutsideVolume(a)
#define IDirectSound3DBuffer_GetPosition(p,a)           (p)->GetPosition(a)
#define IDirectSound3DBuffer_GetMinDistance(p,a)        (p)->GetMinDistance(a)
#define IDirectSound3DBuffer_GetMaxDistance(p,a)        (p)->GetMaxDistance(a)
#define IDirectSound3DBuffer_GetMode(p,a)               (p)->GetMode(a)
#define IDirectSound3DBuffer_GetVelocity(p,a)           (p)->GetVelocity(a)
#define IDirectSound3DBuffer_SetAllParameters(p,a,b)    (p)->SetAllParameters(a,b)
#define IDirectSound3DBuffer_SetConeAngles(p,a,b,c)     (p)->SetConeAngles(a,b,c)
#define IDirectSound3DBuffer_SetConeOrientation(p,a,b,c,d) (p)->SetConeOrientation(a,b,c,d)
#define IDirectSound3DBuffer_SetConeOutsideVolume(p,a,b)(p)->SetConeOutsideVolume(a,b)
#define IDirectSound3DBuffer_SetPosition(p,a,b,c,d)     (p)->SetPosition(a,b,c,d)
#define IDirectSound3DBuffer_SetMinDistance(p,a,b)      (p)->SetMinDistance(a,b)
#define IDirectSound3DBuffer_SetMaxDistance(p,a,b)      (p)->SetMaxDistance(a,b)
#define IDirectSound3DBuffer_SetMode(p,a,b)             (p)->SetMode(a,b)
#define IDirectSound3DBuffer_SetVelocity(p,a,b,c,d)     (p)->SetVelocity(a,b,c,d)
#endif

#endif


/*
 * Return Codes
 */

#define DS_OK                           0

/*
 * The call failed because resources (such as a priority level)
 *  were already being used by another caller.
 */
#define DSERR_ALLOCATED                 MAKE_DSHRESULT( 10 )
/*
 * The control (vol,pan,etc.) requested by the caller is not available.
 */
#define DSERR_CONTROLUNAVAIL            MAKE_DSHRESULT( 30 )
/*
 * An invalid parameter was passed to the returning function
 */
#define DSERR_INVALIDPARAM              E_INVALIDARG
/*
 * This call is not valid for the current state of this object
 */
#define DSERR_INVALIDCALL               MAKE_DSHRESULT( 50 )
/*
 * An undetermined error occured inside the DSound subsystem
 */
#define DSERR_GENERIC                   E_FAIL
/*
 * The caller does not have the priority level required for the function to
 * succeed.
 */
#define DSERR_PRIOLEVELNEEDED           MAKE_DSHRESULT( 70 )
/*
 * The DSound subsystem couldn't allocate sufficient memory to complete the
 * caller's request.
 */
#define DSERR_OUTOFMEMORY               E_OUTOFMEMORY
/*
 * The specified WAVE format is not supported
 */
#define DSERR_BADFORMAT                 MAKE_DSHRESULT( 100 )
/*
 * The function called is not supported at this time
 */
#define DSERR_UNSUPPORTED               E_NOTIMPL
/*
 * No sound driver is available for use
 */
#define DSERR_NODRIVER                  MAKE_DSHRESULT( 120 )
/*
 * This object is already initialized
 */
#define DSERR_ALREADYINITIALIZED        MAKE_DSHRESULT( 130 )
/*
 * This object does not support aggregation
 */
#define DSERR_NOAGGREGATION             CLASS_E_NOAGGREGATION
/*
 * The buffer memory has been lost, and must be Restored.
 */
#define DSERR_BUFFERLOST                MAKE_DSHRESULT( 150 )
/*
 * Another app has a higher priority level, preventing this call from
 * succeeding.
 */
#define DSERR_OTHERAPPHASPRIO           MAKE_DSHRESULT( 160 )
/*
 * The Initialize() member on the Direct Sound Object has not been
 * called or called successfully before calls to other members.
 */
#define DSERR_UNINITIALIZED             MAKE_DSHRESULT( 170 )




//==========================================================================;
//
//                               Flags...
//
//==========================================================================;

#define DSCAPS_PRIMARYMONO          0x00000001
#define DSCAPS_PRIMARYSTEREO        0x00000002
#define DSCAPS_PRIMARY8BIT          0x00000004
#define DSCAPS_PRIMARY16BIT         0x00000008
#define DSCAPS_CONTINUOUSRATE       0x00000010
#define DSCAPS_EMULDRIVER           0x00000020
#define DSCAPS_CERTIFIED            0x00000040
#define DSCAPS_SECONDARYMONO        0x00000100
#define DSCAPS_SECONDARYSTEREO      0x00000200
#define DSCAPS_SECONDARY8BIT        0x00000400
#define DSCAPS_SECONDARY16BIT       0x00000800



#define DSBPLAY_LOOPING                 0x00000001

          
#define DSBSTATUS_PLAYING           0x00000001
#define DSBSTATUS_BUFFERLOST        0x00000002
#define DSBSTATUS_LOOPING           0x00000004
         

#define DSBLOCK_FROMWRITECURSOR         0x00000001



#define DSSCL_NORMAL                1
#define DSSCL_PRIORITY              2
#define DSSCL_EXCLUSIVE             3
#define DSSCL_WRITEPRIMARY          4



// flags for IDirectSound3DBuffer::SetMode
#define DS3DMODE_NORMAL         0       // default must be 0
#define DS3DMODE_HEADRELATIVE   1
#define DS3DMODE_DISABLE        2

// flags for dwApply parameter of some 3D functions
#define DS3D_IMMEDIATE          0
#define DS3D_DEFERRED           1

// default values for 3d factors
#define DS3D_DEFAULTDISTANCEFACTOR      1.0f
#define DS3D_DEFAULTROLLOFFFACTOR       1.0f
#define DS3D_DEFAULTDOPPLERFACTOR       1.0f

#define DSBCAPS_PRIMARYBUFFER       0x00000001
#define DSBCAPS_STATIC              0x00000002
#define DSBCAPS_LOCHARDWARE         0x00000004
#define DSBCAPS_LOCSOFTWARE         0x00000008
#define DSBCAPS_CTRL3D              0x00000010
#define DSBCAPS_CTRLFREQUENCY       0x00000020
#define DSBCAPS_CTRLPAN             0x00000040
#define DSBCAPS_CTRLVOLUME          0x00000080
#define DSBCAPS_CTRLDEFAULT         0x000000E0  // Pan + volume + frequency.
#define DSBCAPS_CTRLALL             0x000000F0  // All control capabilities
#define DSBCAPS_STICKYFOCUS         0x00004000
#define DSBCAPS_GLOBALFOCUS         0x00008000 
#define DSBCAPS_GETCURRENTPOSITION2 0x00010000  // More accurate play cursor under emulation




#define DSSPEAKER_HEADPHONE     1
#define DSSPEAKER_MONO          2
#define DSSPEAKER_QUAD          3
#define DSSPEAKER_STEREO        4
#define DSSPEAKER_SURROUND      5






#ifdef __cplusplus
};
#endif

#endif  /* __DSOUND_INCLUDED__ */
