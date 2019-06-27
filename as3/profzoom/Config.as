package profzoom {
	public class Config {
		/**
		 * Version number.
		 */
		public static const version: String = '1.0.1';

		/**
		 * Path prefix, defaults same as in the binary.
		 */
		public static var prefix: String = '/.profzoom/';

		/**
		 * This is not an instantiable class.
		 *
		 * @private
		 */
		public function Config() {
			throw new Error('Config is a singleton');
		}
	}
}
