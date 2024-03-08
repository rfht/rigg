using System;
using System.IO;

namespace MonoInvisible {
	class MI {
		static void Main(string[] args) {
			string LocalDir = Environment.GetEnvironmentVariable("HOME")
					  + "/.local";
			Console.WriteLine("Checking that {0} is invisible", LocalDir);
			Environment.Exit( Directory.Exists(LocalDir) ? 1 : 0 );
		}
	}
}
