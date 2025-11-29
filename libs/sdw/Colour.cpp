#include "Colour.h"
#include <utility>

Colour::Colour() = default;
Colour::Colour(int r, int g, int b) : red(r), green(g), blue(b) {}
Colour::Colour(std::string n, int r, int g, int b) :
		name(std::move(n)),
		red(r), green(g), blue(b) {}


std::ostream &operator<<(std::ostream &os, const Colour &colour) {
	os << colour.name << " ["
	   << colour.red << ", "
	   << colour.green << ", "
	   << colour.blue << "]";
	return os;
}

uint32_t convertColourToInt(Colour colourObject) {

	uint32_t colourAsInt = (255 << 24) + (int(colourObject.red) << 16) + (int(colourObject.green) << 8) + int(colourObject.blue);
	return colourAsInt;
}
