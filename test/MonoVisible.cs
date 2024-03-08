using System;
using System.IO;

namespace MonoVisible {
	class MV {
		static void Main(string[] args) {
			string LocalShareDir = Environment.GetEnvironmentVariable("HOME")
					       + "/.local/share";
			Console.WriteLine("Checking that {0} is visible", LocalShareDir);
			Environment.Exit( Directory.Exists(LocalShareDir) ? 0 : 1 );
		}
	}
}
