using System;
using System.IO;

namespace MonoVisible {
	class MV {
		static void Main(string[] args) {
			Environment.Exit( Directory.Exists("/tmp") ? 0 : 1 );
		}
	}
}
