// Cryptography functions
RegisterAlias("crypt.base64decode", "crypt.base64decode", ConventionType::SNC, "Decode base64 data");
RegisterAlias("crypt.base64encode", "crypt.base64encode", ConventionType::SNC, "Encode data as base64");
RegisterAlias("crypt.decrypt", "crypt.decrypt", ConventionType::SNC, "Decrypt data");
RegisterAlias("crypt.encrypt", "crypt.encrypt", ConventionType::SNC, "Encrypt data");
RegisterAlias("crypt.generatebytes", "crypt.generatebytes", ConventionType::SNC, "Generate random bytes");
RegisterAlias("crypt.generatekey", "crypt.generatekey", ConventionType::SNC, "Generate a cryptographic key");
RegisterAlias("crypt.hash", "crypt.hash", ConventionType::SNC, "Hash data");

// Debug functions
RegisterAlias("debug.getconstant", "debug.getconstant", ConventionType::SNC, "Get a constant from a function");
RegisterAlias("debug.getconstants", "debug.getconstants", ConventionType::SNC, "Get all constants from a function");
RegisterAlias("debug.getinfo", "debug.getinfo", ConventionType::SNC, "Get information about a function");
RegisterAlias("debug.getproto", "debug.getproto", ConventionType::SNC, "Get a proto from a function");
RegisterAlias("debug.getprotos", "debug.getprotos", ConventionType::SNC, "Get all protos from a function");
RegisterAlias("debug.getstack", "debug.getstack", ConventionType::SNC, "Get the stack of a thread");
RegisterAlias("debug.getupvalue", "debug.getupvalue", ConventionType::SNC, "Get an upvalue from a function");
RegisterAlias("debug.getupvalues", "debug.getupvalues", ConventionType::SNC, "Get all upvalues from a function");
RegisterAlias("debug.print", "debug.print", ConventionType::SNC, "Print debug information");
RegisterAlias("debug.setconstant", "debug.setconstant", ConventionType::SNC, "Set a constant in a function");
RegisterAlias("debug.setstack", "debug.setstack", ConventionType::SNC, "Set a value in the stack");
RegisterAlias("debug.setupvalue", "debug.setupvalue", ConventionType::SNC, "Set an upvalue in a function");

// File system functions
RegisterAlias("appendfile", "appendfile", ConventionType::SNC, "Append to a file");
RegisterAlias("delfile", "delfile", ConventionType::SNC, "Delete a file");
RegisterAlias("delfolder", "delfolder", ConventionType::SNC, "Delete a folder");
RegisterAlias("dofile", "dofile", ConventionType::SNC, "Execute a file");
RegisterAlias("isfile", "isfile", ConventionType::SNC, "Check if a file exists");
RegisterAlias("isfolder", "isfolder", ConventionType::SNC, "Check if a folder exists");
RegisterAlias("listfiles", "listfiles", ConventionType::SNC, "List files in a folder");
RegisterAlias("loadfile", "loadfile", ConventionType::SNC, "Load a file as a function");
RegisterAlias("makefolder", "makefolder", ConventionType::SNC, "Create a folder");
RegisterAlias("readfile", "readfile", ConventionType::SNC, "Read a file");
RegisterAlias("writefile", "writefile", ConventionType::SNC, "Write to a file");

// Instance interaction functions
RegisterAlias("fireclickdetector", "fireclickdetector", ConventionType::SNC, "Fire a click detector");
RegisterAlias("fireproximityprompt", "fireproximityprompt", ConventionType::SNC, "Fire a proximity prompt");
RegisterAlias("firesignal", "firesignal", ConventionType::SNC, "Fire a signal");
RegisterAlias("firetouchinterest", "firetouchinterest", ConventionType::SNC, "Fire a touch interest");
RegisterAlias("getcallbackvalue", "getcallbackvalue", ConventionType::SNC, "Get a callback value");
RegisterAlias("getconnections", "getconnections", ConventionType::SNC, "Get connections from a signal");
RegisterAlias("getcustomasset", "getcustomasset", ConventionType::SNC, "Get a custom asset");
RegisterAlias("gethiddenproperty", "gethiddenproperty", ConventionType::SNC, "Get a hidden property");
RegisterAlias("gethui", "gethui", ConventionType::SNC, "Get the hidden UI");
RegisterAlias("getinstances", "getinstances", ConventionType::SNC, "Get all instances");
RegisterAlias("getnilinstances", "getnilinstances", ConventionType::SNC, "Get nil instances");
RegisterAlias("isrbxactive", "isrbxactive", ConventionType::SNC, "Check if Roblox is active");
RegisterAlias("sethiddenproperty", "sethiddenproperty", ConventionType::SNC, "Set a hidden property");

// Mouse input functions
RegisterAlias("mouse1click", "mouse1click", ConventionType::SNC, "Simulate a left mouse click");
RegisterAlias("mouse1press", "mouse1press", ConventionType::SNC, "Simulate a left mouse press");
RegisterAlias("mouse1release", "mouse1release", ConventionType::SNC, "Simulate a left mouse release");
RegisterAlias("mouse2click", "mouse2click", ConventionType::SNC, "Simulate a right mouse click");
RegisterAlias("mouse2press", "mouse2press", ConventionType::SNC, "Simulate a right mouse press");
RegisterAlias("mouse2release", "mouse2release", ConventionType::SNC, "Simulate a right mouse release");
RegisterAlias("mousemoveabs", "mousemoveabs", ConventionType::SNC, "Move the mouse to absolute coordinates");
RegisterAlias("mousemoverel", "mousemoverel", ConventionType::SNC, "Move the mouse by relative coordinates");
RegisterAlias("mousescroll", "mousescroll", ConventionType::SNC, "Simulate mouse scrolling");
