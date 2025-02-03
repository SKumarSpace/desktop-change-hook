package main

import (
	"fmt"
	"log"

	"github.com/go-ole/go-ole"
	"github.com/go-ole/go-ole/oleutil"
)

func main() {
	// Initialize COM library
	err := ole.CoInitialize(0)
	if err != nil {
		log.Fatal("Error initializing COM library:", err)
	}
	defer ole.CoUninitialize()

	// Define CLSID and IID for IVirtualDesktopManagerInternal
	clsid := ole.NewGUID("AA509086-5CA9-4C25-8F95-589D3C07B48A")
	//clsid := ole.NewGUID("C5E0CDCA-7B6E-41B2-9FC4-D93975CC467B")
	//iid := ole.NewGUID("53F5CA0B-158F-4124-900C-057158060B27")
	iid := ole.IID_IUnknown

	// Create an instance of IVirtualDesktopManagerInternal
	unknown, err := ole.CreateInstance(clsid, iid)
	if err != nil {
		log.Fatal("Error creating IVirtualDesktopManagerInternal instance:", err)
	}
	defer unknown.Release()

	// Query for the IDispatch interface
	dispatch, err := unknown.QueryInterface(ole.IID_IUnknown)
	if err != nil {
		log.Fatal("Error querying for IDispatch:", err)
	}
	defer dispatch.Release()

	// Call the GetCount method on IVirtualDesktopManagerInternal
	result, err := oleutil.CallMethod(dispatch, "GetCount")
	if err != nil {
		log.Fatal("Error calling GetCount method:", err)
	}
	fmt.Println("Number of virtual desktops:", result.Val)
}
