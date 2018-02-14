#pragma once

#include <string>
#include <vector>

namespace C9
{
	/*
	Get the list of volumes on the local computer and provide methods to get
	*/

	class C9GetVolumeList
	{
	public:
		C9GetVolumeList();
		~C9GetVolumeList();

	public:
		/* Return the number of volumes that we located. */
		size_t
			size() const;

		std::wstring
			volume(size_t item) const;

	private:
		std::vector<std::wstring> volumes;
	};
}
