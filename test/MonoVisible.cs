using System;
using System.IO;

namespace MonoVisible {
	class MV {
		static void Main(string[] args) {
			string testDir = "/tmp";
			Console.WriteLine("Checking that {0} is visible", testDir);
			Environment.Exit( Directory.Exists(testDir) ? 0 : 1 );
		}
	}
}
