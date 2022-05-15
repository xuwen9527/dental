#include <uuid.h>
#include <stdio.h>
#include <iostream>
#ifdef WIN32
#include <objbase.h>
#else
#include <uuid.hxx>
#endif
#define MAX_LEN 128

namespace Dental {
	std::string createUUID() {
		char szuuid[MAX_LEN] = { 0 };
#ifdef WIN32
		GUID guid;
		auto result = CoCreateGuid(&guid);
		_snprintf_s(
			szuuid, sizeof(szuuid),
			"{%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x}",
			guid.Data1, guid.Data2, guid.Data3,
			guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
			guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]
		);
#else
		uuid_t uuid;
		uuid_generate(uuid);
		uuid_unparse(uuid, szuuid);
#endif // WIN32
		return std::string(szuuid);
	}
}
