#include "core.h"
#include "core_crypto.h"
#include "core_filesystem.h"
#include <stdio.h>
#include <stdlib.h>

using namespace core;

static void write_header(crypto_t::key_t const& pub_key, WCHAR const* path) {
	filesystem::file_t file;
	if (file.open(GENERIC_WRITE, OPEN_ALWAYS, path) == false) {
		return;
	}

	file.print("static const unsigned char public_key[] = {");

	u32 comma = 0;
	for (u32 i = 0; i < pub_key.size; ++i) {
		if (comma) {
			file.print(", ");
		} else {
			comma = 1;
		}

		if ((i % 12) == 0) {
			file.print("\r\n\t");
		}

		file.print("0x%.2X", pub_key.data[i]);
	}

	file.print("\r\n};\r\n");
	file.close();
}

// Signing tool.
// Usage:
// If the private key don't exist in the current directory, new pub/private pair will be generated.
// It will generate update.sig if update.dll is in the current directory.
int main(int argc, char** argv) {

	static WCHAR const update_file_path[] = L"update.dll";
	static WCHAR const update_sig_path[] = L"update.sig";
	static WCHAR const header_path[] = L"pub.h";
	static WCHAR const pub_key_path[] = L"pub.key";
	static WCHAR const pri_key_path[] = L"pri.key";

	crypto_t crypto;
	crypto_t::key_t pub_key, pri_key;

	if (filesystem::file_t file; file.open(GENERIC_READ, OPEN_EXISTING, pri_key_path)) {
		pri_key.size = file.read(pri_key.data, sizeof(pri_key.data));
	}

	if (pri_key.size == 0) {
		if (crypto.gen_keys(&pub_key, &pri_key)) {
			filesystem::write(pub_key.data, pub_key.size, pub_key_path);
			filesystem::write(pri_key.data, pri_key.size, pri_key_path);

			write_header(pub_key, header_path);

		} else {
			printf("error: couldn't generate signing keys.\n");
			return EXIT_FAILURE;
		}
	}

	if (pri_key.size == 0) {
		printf("error: no private key available.\n");
		return EXIT_FAILURE;
	}

	filesystem::buffer_t update_file;
	if (filesystem::read(&update_file, update_file_path) == false) {
		printf("error: %S is missing.\n", update_file_path);
		return EXIT_FAILURE;
	}


	BYTE sig[512];
	if (crypto.sign_sig(sig, sizeof(sig), update_file.data, update_file.size, pri_key.data, pri_key.size) == false) {
		printf("error: couldn't sign %S.", update_file_path);
		return EXIT_FAILURE;
	}

	filesystem::write(sig, sizeof(sig), update_sig_path);

	return EXIT_SUCCESS;
}
