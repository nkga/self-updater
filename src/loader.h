#pragma once

namespace loader {

// Valid actions for the client update callback.
enum class action_t {
	none,
	reload,
	exit,
};

}
