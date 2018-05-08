#include "core_crypto.h"
#include <assert.h>

namespace core {

crypto_t::crypto_t() {
	if (CryptAcquireContextW(&hprov, 0, MS_ENH_RSA_AES_PROV, PROV_RSA_AES, 0) == FALSE) {
		hprov = 0;
	}
}

crypto_t::~crypto_t() {
	if (hprov) {
		CryptReleaseContext(hprov, 0);
		hprov = 0;
	}
}

bool crypto_t::gen_keys(key_t* pub, key_t* pri) {
	assert(pub);
	assert(pri);

	if (hprov == 0) {
		return false;
	}

	HCRYPTKEY key;
	if (CryptGenKey(hprov, AT_KEYEXCHANGE, (4096 << 16) | CRYPT_ARCHIVABLE | CRYPT_EXPORTABLE, &key) == false) {
		return false;
	}

	// You can feed data in as nullptr first to query the exact size needed, but
	// the fixed size buffer is nicer to work with.

	BOOL res = TRUE;

	pub->size = sizeof(pub->data);
	res &= CryptExportKey(key, 0, PUBLICKEYBLOB, 0, pub->data, &pub->size);

	pri->size = sizeof(pri->data);
	res &= CryptExportKey(key, 0, PRIVATEKEYBLOB, 0, pri->data, &pri->size);

	CryptDestroyKey(key);

	return res;
}

u64 crypto_t::gen_token() {
	// NOTE:
	// This kind behavior isn't acceptable for secure applications.
	// If the system RNG isn't available it should be treated as an error.
	// In the demo's case, it doesn't break the security of the application - just the denial of service protection.
	// Keeping the API simpler is acceptable in this case.

	u64 val = 0;

	if (hprov) {
		CryptGenRandom(hprov, sizeof(val), (BYTE*)&val);
	}

	return val;
}

bool crypto_t::sign_sig(BYTE* sig, u32 sig_bytes, BYTE const* data, u32 data_size, BYTE const* pri_key, u32 pri_key_bytes) {
	if (hprov == 0) {
		return false;
	}

	bool res = false;

	if (HCRYPTKEY hkey; CryptImportKey(hprov, pri_key, pri_key_bytes, 0, 0, &hkey)) {
		if (HCRYPTHASH hash; CryptCreateHash(hprov, CALG_SHA_256, 0, 0, &hash)) {
			if (CryptHashData(hash, (BYTE*)data, data_size, 0)) {
				DWORD len = sig_bytes;
				res = CryptSignHashW(hash, AT_KEYEXCHANGE, 0, 0, sig, &len);
			}

			CryptDestroyHash(hash);
		}

		CryptDestroyKey(hkey);
	}

	return res;
}

bool crypto_t::test_sig(void const* sig, u32 sig_bytes, void const* src, u32 src_bytes, BYTE const* pub_key, u32 pub_key_bytes) {
	if (hprov == 0) {
		return false;
	}

	bool res = false;

	if (HCRYPTKEY hkey; CryptImportKey(hprov, pub_key, pub_key_bytes, 0, 0, &hkey)) {
		if (HCRYPTHASH hash; CryptCreateHash(hprov, CALG_SHA_256, 0, 0, &hash)) {
			if (CryptHashData(hash, (BYTE const*)src, src_bytes, 0)) {
				res = CryptVerifySignatureW(hash, (BYTE const*)sig, sig_bytes, hkey, 0, 0);
			}

			CryptDestroyHash(hash);
		}

		CryptDestroyKey(hkey);
	}

	return res;
}

}
