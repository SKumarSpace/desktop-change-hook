// gcc desktop-change-hook.c -o desktop-change-hook.exe -lole32 -luuid -lcomdlg32
#include <windows.h>
#include <stdio.h>
#include <Shobjidl.h> // Required for virtual desktop interfaces

// GUIDs for Virtual Desktop APIs (manually defined)
DEFINE_GUID(IID_IVirtualDesktopNotification, 0xcd403abf, 0xfe94, 0x4c67, 0x80, 0x07, 0x8a, 0x80, 0x13, 0x39, 0xd7, 0x4c);

// Define IVirtualDesktopNotification (manually)
typedef struct IVirtualDesktopNotificationVtbl
{
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(void *This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(void *This);
    ULONG(STDMETHODCALLTYPE *Release)(void *This);
    HRESULT(STDMETHODCALLTYPE *VirtualDesktopChanged)(void *This, void *pOldDesktop, void *pNewDesktop);
} IVirtualDesktopNotificationVtbl;

typedef struct IVirtualDesktopNotification
{
    IVirtualDesktopNotificationVtbl *lpVtbl;
} IVirtualDesktopNotification;

// Define the GUIDs (manually found from Windows internals)
DEFINE_GUID(IID_IVirtualDesktop, 0xf31574d6, 0xb682, 0x4cdc, 0xb9, 0x26, 0x0a, 0xa0, 0x7f, 0x7f, 0x23, 0x8d);

// Define IVirtualDesktop Interface
typedef struct IVirtualDesktopVtbl
{
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(void *This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(void *This);
    ULONG(STDMETHODCALLTYPE *Release)(void *This);
    HRESULT(STDMETHODCALLTYPE *GetID)(void *This, GUID *desktopId);
} IVirtualDesktopVtbl;

typedef struct IVirtualDesktop
{
    IVirtualDesktopVtbl *lpVtbl;
} IVirtualDesktop;

// Define GUID for IVirtualDesktopManager (manually reverse-engineered)
DEFINE_GUID(IID_IVirtualDesktopManager, 0xa5cd92ff, 0x29be, 0x454c, 0x8d, 0x04, 0xd8, 0x08, 0x79, 0xb5, 0x5e, 0x98);

// Define IVirtualDesktopManager interface
typedef struct IVirtualDesktopManagerVtbl
{
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(void *This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(void *This);
    ULONG(STDMETHODCALLTYPE *Release)(void *This);
    HRESULT(STDMETHODCALLTYPE *IsWindowOnCurrentVirtualDesktop)(void *This, HWND hwnd, BOOL *pfOnCurrentDesktop);
    HRESULT(STDMETHODCALLTYPE *GetVirtualDesktopForWindow)(void *This, HWND hwnd, void **ppDesktop);
    HRESULT(STDMETHODCALLTYPE *GetWindowForVirtualDesktop)(void *This, void *pDesktop, HWND *hwnd);
    HRESULT(STDMETHODCALLTYPE *SwitchToDesktop)(void *This, void *pDesktop);
    HRESULT(STDMETHODCALLTYPE *GetDesktops)(void *This, void ***ppDesktops, UINT *pCount);
    HRESULT(STDMETHODCALLTYPE *CreateDesktop)(void *This, void **ppDesktop);
    HRESULT(STDMETHODCALLTYPE *RemoveDesktop)(void *This, void *pDesktop);
} IVirtualDesktopManagerVtbl;

typedef struct IVirtualDesktopManager
{
    IVirtualDesktopManagerVtbl *lpVtbl;
} IVirtualDesktopManager;

// Define GUIDs for Virtual Desktop Manager
DEFINE_GUID(CLSID_VirtualDesktopManager,
            0xF31574D6, 0x29BE, 0x4CDC, 0xB9, 0x26, 0x0A, 0xA0, 0x7F, 0x7F, 0x23, 0x8D);

DEFINE_GUID(IID_IVirtualDesktopManager,
            0xA5CD92FF, 0x29BE, 0x454C, 0x8D, 0x04, 0xD8, 0x08, 0x79, 0xB5, 0x5E, 0x98);

// Define other needed GUIDs
DEFINE_GUID(IID_IUnknown,
            0x00000000, 0x0000, 0x0000, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46);

#include <objbase.h>

// Define the GUID for IVirtualDesktopNotificationService (reverse-engineered)
// Note: This GUID might vary; this is one widely-used value.
DEFINE_GUID(IID_IVirtualDesktopNotificationService,
            0xa501fdec, 0x4a09, 0x464c, 0xae, 0x4d, 0x45, 0xb3, 0xe5, 0xe4, 0xe6, 0x3d);

typedef struct IVirtualDesktopNotificationServiceVtbl
{
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(void *This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(void *This);
    ULONG(STDMETHODCALLTYPE *Release)(void *This);
    HRESULT(STDMETHODCALLTYPE *Register)(void *This, struct IVirtualDesktopNotification *pNotification, UINT *cookie);
    HRESULT(STDMETHODCALLTYPE *Unregister)(void *This, UINT cookie);
} IVirtualDesktopNotificationServiceVtbl;

typedef struct IVirtualDesktopNotificationService
{
    IVirtualDesktopNotificationServiceVtbl *lpVtbl;
} IVirtualDesktopNotificationService;

// Global reference to VirtualDesktopNotification object
IVirtualDesktopNotification *g_pDesktopNotification = NULL;

// UUIDs for the virtual desktop COM interfaces (needed for runtime binding)
DEFINE_GUID(CLSID_VirtualDesktopManager, 0xaa509086, 0x5ca9, 0x4c25, 0x8f, 0x95, 0x58, 0x9d, 0x3c, 0x07, 0x68, 0x78);
DEFINE_GUID(IID_IVirtualDesktopManager, 0xa5cd92ff, 0x29be, 0x454c, 0x8d, 0x04, 0xd8, 0x08, 0x79, 0xb5, 0x5e, 0x98);

// Custom class implementing IVirtualDesktopNotification
typedef struct VirtualDesktopNotification
{
    IVirtualDesktopNotificationVtbl *lpVtbl;
    ULONG refCount;
} VirtualDesktopNotification;

// VirtualDesktopNotification methods
HRESULT STDMETHODCALLTYPE QueryInterface(IVirtualDesktopNotification *This, REFIID riid, void **ppvObject)
{
    if (IsEqualIID(riid, &IID_IUnknown))
    {
        *ppvObject = This;
    }
    else
    {
        *ppvObject = NULL;
        return E_NOINTERFACE;
    }
    This->lpVtbl->AddRef(This);
    return S_OK;
}

// HEre?
//  Declaration for GetVirtualDesktopNotificationService
HRESULT GetVirtualDesktopNotificationService(IVirtualDesktopManager *pDesktopManager, IVirtualDesktopNotificationService **ppNotificationService);

ULONG STDMETHODCALLTYPE AddRef(IVirtualDesktopNotification *This)
{
    return ++((VirtualDesktopNotification *)This)->refCount;
}

ULONG STDMETHODCALLTYPE Release(IVirtualDesktopNotification *This)
{
    VirtualDesktopNotification *self = (VirtualDesktopNotification *)This;
    if (--self->refCount == 0)
    {
        free(self);
        return 0;
    }
    return self->refCount;
}

// Callback when desktop is switched
HRESULT STDMETHODCALLTYPE VirtualDesktopChanged(IVirtualDesktopNotification *This, IVirtualDesktop *pOldDesktop, IVirtualDesktop *pNewDesktop)
{
    printf("Virtual desktop changed!\n");
    return S_OK;
}

// Helper to register our notification handler
HRESULT RegisterVirtualDesktopNotifications()
{
    IVirtualDesktopManager *pDesktopManager = NULL;
    HRESULT hr = CoCreateInstance(&CLSID_VirtualDesktopManager, NULL, CLSCTX_ALL, &IID_IVirtualDesktopManager, (void **)&pDesktopManager);
    if (FAILED(hr))
    {
        printf("Failed to create VirtualDesktopManager instance: 0x%lx\n", hr);
        return hr;
    }

    VirtualDesktopNotification *pNotification = (VirtualDesktopNotification *)malloc(sizeof(VirtualDesktopNotification));
    pNotification->lpVtbl = (IVirtualDesktopNotificationVtbl *)malloc(sizeof(IVirtualDesktopNotificationVtbl));
    pNotification->lpVtbl->QueryInterface = QueryInterface;
    pNotification->lpVtbl->AddRef = AddRef;
    pNotification->lpVtbl->Release = Release;
    pNotification->lpVtbl->VirtualDesktopChanged = VirtualDesktopChanged;
    pNotification->refCount = 1;

    IVirtualDesktopNotificationService *pNotificationService = NULL;
    // Assume `GetVirtualDesktopNotificationService()` gets the required service
    hr = GetVirtualDesktopNotificationService(pDesktopManager, &pNotificationService);
    if (FAILED(hr))
    {
        printf("Failed to get VirtualDesktopNotificationService: 0x%lx\n", hr);
        return hr;
    }

    UINT cookie;
    hr = pNotificationService->lpVtbl->Register(pNotificationService, (IVirtualDesktopNotification *)pNotification, &cookie);
    if (FAILED(hr))
    {
        printf("Failed to register notification: 0x%lx\n", hr);
        return hr;
    }

    printf("Virtual desktop switch notifications registered!\n");
    return S_OK;
}

int main()
{
    CoInitialize(NULL); // Initialize COM
    RegisterVirtualDesktopNotifications();

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    CoUninitialize(); // Cleanup
    return 0;
}
