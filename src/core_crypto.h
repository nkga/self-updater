#pragma once
#include "core.h"
#include <Windows.h>
#include <wincrypt.h>

namespace core {

class crypto_t {
public:
	// Cryptographic key used for signing or verification.
	struct key_t {
		DWORD size;
		BYTE data[4096];

		inline key_t() : size(0) {
		}
	};

	// Initializes the crytographic context.
	crypto_t();

	// Releases the crytographic context.
	~crypto_t();

	// Generates a private/public key pair.
	bool gen_keys(key_t* pub, key_t* pri);

	// Generates a random token.
	u64 gen_token();

	// Returns true if the signing succeded.
	bool sign_sig(BYTE* sig, u32 sig_bytes, BYTE const* data, u32 data_size, BYTE const* pri_key, u32 pri_key_bytes);

	// Returns true if the signature is valid for the given data.
	bool test_sig(void const* sig, u32 sig_bytes, void const* src, u32 src_bytes, BYTE const* pub_key, u32 pub_key_bytes);

protected:
	HCRYPTPROV hprov;
};

}
