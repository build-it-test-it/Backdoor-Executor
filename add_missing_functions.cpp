// Cryptography functions
RegisterAlias("crypt.base64decode", "crypt.base64decode", ConventionType::UNC, "Decode base64 data");
RegisterAlias("crypt.base64encode", "crypt.base64encode", ConventionType::UNC, "Encode data as base64");
RegisterAlias("crypt.decrypt", "crypt.decrypt", ConventionType::UNC, "Decrypt data");
RegisterAlias("crypt.encrypt", "crypt.encrypt", ConventionType::UNC, "Encrypt data");
RegisterAlias("crypt.generatebytes", "crypt.generatebytes", ConventionType::UNC, "Generate random bytes");
RegisterAlias("crypt.generatekey", "crypt.generatekey", ConventionType::UNC, "Generate a cryptographic key");
RegisterAlias("crypt.hash", "crypt.hash", ConventionType::UNC, "Hash data");

// Debug functions
RegisterAlias("debug.getconstant", "debug.getconstant", ConventionType::UNC, "Get a constant from a function");
RegisterAlias("debug.getconstants", "debug.getconstants", ConventionType::UNC, "Get all constants from a function");
RegisterAlias("debug.getinfo", "debug.getinfo", ConventionType::UNC, "Get information about a function");
RegisterAlias("debug.getproto", "debug.getproto", ConventionType::UNC, "Get a proto from a function");
RegisterAlias("debug.getprotos", "debug.getprotos", ConventionType::UNC, "Get all protos from a function");
RegisterAlias("debug.getstack", "debug.getstack", ConventionType::UNC, "Get the stack of a thread");
RegisterAlias("debug.getupvalue", "debug.getupvalue", ConventionType::UNC, "Get an upvalue from a function");
RegisterAlias("debug.getupvalues", "debug.getupvalues", ConventionType::UNC, "Get all upvalues from a function");
RegisterAlias("debug.print", "debug.print", ConventionType::UNC, "Print debug information");
RegisterAlias("debug.setconstant", "debug.setconstant", ConventionType::UNC, "Set a constant in a function");
RegisterAlias("debug.setstack", "debug.setstack", ConventionType::UNC, "Set a value in the stack");
RegisterAlias("debug.setupvalue", "debug.setupvalue", ConventionType::UNC, "Set an upvalue in a function");

// File system functions
RegisterAlias("appendfile", "appendfile", ConventionType::UNC, "Append to a file");
RegisterAlias("delfile", "delfile", ConventionType::UNC, "Delete a file");
RegisterAlias("delfolder", "delfolder", ConventionType::UNC, "Delete a folder");
RegisterAlias("dofile", "dofile", ConventionType::UNC, "Execute a file");
RegisterAlias("isfile", "isfile", ConventionType::UNC, "Check if a file exists");
RegisterAlias("isfolder", "isfolder", ConventionType::UNC, "Check if a folder exists");
RegisterAlias("listfiles", "listfiles", ConventionType::UNC, "List files in a folder");
RegisterAlias("loadfile", "loadfile", ConventionType::UNC, "Load a file as a function");
RegisterAlias("makefolder", "makefolder", ConventionType::UNC, "Create a folder");
RegisterAlias("readfile", "readfile", ConventionType::UNC, "Read a file");
RegisterAlias("writefile", "writefile", ConventionType::UNC, "Write to a file");

// Instance interaction functions
RegisterAlias("fireclickdetector", "fireclickdetector", ConventionType::UNC, "Fire a click detector");
RegisterAlias("fireproximityprompt", "fireproximityprompt", ConventionType::UNC, "Fire a proximity prompt");
RegisterAlias("firesignal", "firesignal", ConventionType::UNC, "Fire a signal");
RegisterAlias("firetouchinterest", "firetouchinterest", ConventionType::UNC, "Fire a touch interest");
RegisterAlias("getcallbackvalue", "getcallbackvalue", ConventionType::UNC, "Get a callback value");
RegisterAlias("getconnections", "getconnections", ConventionType::UNC, "Get connections from a signal");
RegisterAlias("getcustomasset", "getcustomasset", ConventionType::UNC, "Get a custom asset");
RegisterAlias("gethiddenproperty", "gethiddenproperty", ConventionType::UNC, "Get a hidden property");
RegisterAlias("gethui", "gethui", ConventionType::UNC, "Get the hidden UI");
RegisterAlias("getinstances", "getinstances", ConventionType::UNC, "Get all instances");
RegisterAlias("getnilinstances", "getnilinstances", ConventionType::UNC, "Get nil instances");
RegisterAlias("isrbxactive", "isrbxactive", ConventionType::UNC, "Check if Roblox is active");
RegisterAlias("sethiddenproperty", "sethiddenproperty", ConventionType::UNC, "Set a hidden property");

// Mouse input functions
RegisterAlias("mouse1click", "mouse1click", ConventionType::UNC, "Simulate a left mouse click");
RegisterAlias("mouse1press", "mouse1press", ConventionType::UNC, "Simulate a left mouse press");
RegisterAlias("mouse1release", "mouse1release", ConventionType::UNC, "Simulate a left mouse release");
RegisterAlias("mouse2click", "mouse2click", ConventionType::UNC, "Simulate a right mouse click");
RegisterAlias("mouse2press", "mouse2press", ConventionType::UNC, "Simulate a right mouse press");
RegisterAlias("mouse2release", "mouse2release", ConventionType::UNC, "Simulate a right mouse release");
RegisterAlias("mousemoveabs", "mousemoveabs", ConventionType::UNC, "Move the mouse to absolute coordinates");
RegisterAlias("mousemoverel", "mousemoverel", ConventionType::UNC, "Move the mouse by relative coordinates");
RegisterAlias("mousescroll", "mousescroll", ConventionType::UNC, "Simulate mouse scrolling");
