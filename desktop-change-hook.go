package main

import (
	"log"

	"github.com/go-ole/go-ole"
)

var (
	CLSID_ImmersiveShell               = ole.NewGUID("C2F03A33-21F5-47FA-B4BB-156362A2F239")
	IID_IServiceProvider               = ole.NewGUID("6D5140C1-7436-11CE-8034-00AA006009FA")
	IID_VirtualDesktopManagerInternal  = ole.NewGUID("C5E0CDCA-7B6E-41B2-9FC4-D93975CC467B")
	IID_IVirtualDesktopManagerInternal = ole.NewGUID("AF8DA486-95BB-4460-B3B7-6E7A6B2962B5")
	IVirtualDesktopManagerInternal2    = ole.NewGUID("53F5CA0B-158F-4124-900C-057158060B27")
	// TODO: Need to understand what this IID is for ^
)

func main() {
	// Initialize COM library
	err := ole.CoInitialize(0)
	if err != nil {
		log.Fatal("Error initializing COM library:", err)
	}
	defer ole.CoUninitialize()

	// Get Immersive Shell as IUnknown
	shellUnknown, err := ole.CreateInstance(CLSID_ImmersiveShell, ole.IID_IUnknown)
	if err != nil {
		log.Fatal("error creating shell:", err)
	}
	defer shellUnknown.Release()

	// Query IServiceProvider from Immersive Shell
	serviceProvider, err := shellUnknown.QueryInterface(IID_IServiceProvider)
	if err != nil {
		log.Fatal("Failed to query IServiceProvider:", err)
	}
	defer serviceProvider.Release()

	virtualDesktopManagerInternal, err := serviceProvider.CallMethod("QueryService", IID_VirtualDesktopManagerInternal, IVirtualDesktopManagerInternal2)
	if err != nil {
		log.Fatal("Failed to query IVirtualDesktopManagerInternal:", err)
	}
	defer virtualDesktopManagerInternal.Clear()

	// // Query for the IDispatch interface
	// iid2 := ole.NewGUID("53F5CA0B-158F-4124-900C-057158060B27")
	// dispatch, err := unknown.QueryInterface(iid2)
	// if err != nil {
	// 	log.Fatal("Error querying for IDispatch:", err)
	// }
	// defer dispatch.Release()

	// // Call the GetCount method on IVirtualDesktopManagerInternal
	// result, err := oleutil.CallMethod(dispatch, "GetCount")
	// if err != nil {
	// 	log.Fatal("Error calling GetCount method:", err)
	// }
	// fmt.Println("Number of virtual desktops:", result.Val)
}
