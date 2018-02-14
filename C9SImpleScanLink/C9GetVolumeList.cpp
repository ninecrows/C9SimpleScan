#include "stdafx.h"
#include "C9GetVolumeList.h"
#include <windows.h>
#include <memory>

namespace
{
  // Used internally by C9GetVolumeList to manage the native handle.
  class VolumeHandle
  {
  private:
    // Store the item returned by the first call so we can retrieve it later.
    std::wstring firstItem;
    std::wstring nextItem;

    // Handle used to get subsequent items.
    HANDLE myHandle;

    // Error code (if there's an error)
    DWORD status;

    // Set to true after the find first call so that we can return
    // that result on the first request.
    bool firstTime;

    // Set to true if there are additional results available.
    bool running;

  public:
    inline
    VolumeHandle(void)
    {
      // Assume success
      status = ERROR_SUCCESS;

      // Get started
      const size_t bufferSize(1024);
      std::unique_ptr<wchar_t[]> buffer(new wchar_t[bufferSize]);
      myHandle = ::FindFirstVolume(buffer.get(), bufferSize);

      // If we got the first volume name then save it
      if (myHandle != INVALID_HANDLE_VALUE)
	{
	  firstItem = buffer.get();
	  firstTime = true;
	  running = true;
	}

      // If we failed then save our error code for later.
      else
	{
	  status = ::GetLastError();
	  running = false;

	}
    }

    inline
    ~VolumeHandle(void)
    {
      HANDLE closeThis(InterlockedExchangePointer(&myHandle,
						  INVALID_HANDLE_VALUE));
      if (closeThis != INVALID_HANDLE_VALUE)
	  {
	    FindVolumeClose(closeThis);
	  }
    }

  public:
    inline
    std::wstring
    Result(void)
    {
      std::wstring result;

      // For the first call we return the 
      if (firstTime)
	{
	  firstTime = false;
				
	}
			
      result = firstItem;

      {
	    const size_t bufferSize(1024);
	    std::unique_ptr<wchar_t[]> buffer(new wchar_t[bufferSize]);

	    running = FindNextVolumeW(myHandle, buffer.get(), bufferSize);
	    if (running)
	    {
	    firstItem = buffer.get();
	  }
	else
	  {
	    // Check to see if our error is a non-error error and
	    // leave us with success if that is the case.
	    DWORD checkStatus = ::GetLastError();
	    if (checkStatus != ERROR_NO_MORE_FILES)
	      {
		status = checkStatus;
	      }
	  }
      }
			

      return (result);
    }

    inline
    bool
    isRunning() const
    {
      return running;
    }

    // If anything but ERROR_SUCCESS is here then we encountered some
    // sort of error along the way.
    inline
    DWORD
    getStatus() const
    {
      return (status);
    }

  private:

  };
}

namespace C9
{	
  C9GetVolumeList::C9GetVolumeList()
  {
    VolumeHandle volumeScan;
		
    // If we got the first volume then store it and continue...
    while (volumeScan.isRunning())
      {
	// Save the first item returned as part of the start of scan.
	volumes.push_back(volumeScan.Result());
      }

    // Throw is something failed along the way. Should provide more
    // information in here than we're currently providing.
    if (volumeScan.getStatus() != ERROR_SUCCESS)
      {
			
	throw std::exception("Failed to start walking volume list");
      }
  }


  C9GetVolumeList::~C9GetVolumeList()
  {
  }

  /*
    @brief Return the number of volumes this object found on construction.
  */
  size_t
  C9GetVolumeList::size() const
  {
    return (volumes.size());
  }

  /*
    @brief Hand off the requested volume or throw an exception if the
    request is out of range.
  */
  std::wstring
  C9GetVolumeList::volume(size_t thisOne) const
  {
    if (thisOne < volumes.size())
      {
	return volumes[thisOne];
      }
    else
      {
	throw std::exception("Index out of range");
      }
  }
}
