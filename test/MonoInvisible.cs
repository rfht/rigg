using System;
using System.IO;

namespace MonoInvisible {
	class MI {
		static void Main(string[] args) {
			string testDir = Environment.GetEnvironmentVariable("HOME");
			if (testDir == null) {
				Console.WriteLine("Failed to resolve $HOME");
				Environment.Exit(1);
			}
			Console.WriteLine("Checking that $HOME ({0}) is invisible", testDir);

			/*
			 * return 0 if doesn't exist; then it is hidden by unveil(2)
                         * as intended
                         */
			Environment.Exit( Directory.Exists(testDir) ? 1 : 0 );
		}
	}
}
