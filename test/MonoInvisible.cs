using System;
using System.IO;

namespace MonoInvisible {
	class MI {
		static void Main(string[] args) {
			string testDir = "/home";
			Console.WriteLine("Checking that {0} is invisible", testDir);

			/*
			 * return 0 if doesn't exist; then it is hidden by unveil(2)
                         * as intended
                         */
			Environment.Exit( Directory.Exists(testDir) ? 1 : 0 );
		}
	}
}
