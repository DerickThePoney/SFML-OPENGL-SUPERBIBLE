#pragma once

#define SAFE_DELETE(x) if(x != nullptr) { delete x; x = nullptr; }
#define SAFE_DELETE_ARRAY(x) if(x != nullptr) { delete[] x; x = nullptr; }
