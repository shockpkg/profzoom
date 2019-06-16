package profzoom.commands {
	import flash.utils.ByteArray;
	import profzoom.Command;
	import profzoom.events.CommandDone;

	public class EnvGet extends Command {
		/**
		 * Command hook name.
		 */
		protected static const _HOOK: String = 'envget';

		/**
		 * Environment variable reader.
		 */
		public function EnvGet() {
			super();
		}

		/**
		 * Read environment variable.
		 *
		 * @param name Environment variable name.
		 */
		public function read(name: String): String {
			this._run([name]);
		}

		/**
		 * Handle the response data.
		 *
		 * @param flag Flag value.
		 * @param data Byte data.
		 */
		override protected function _handleData(
			flag: uint,
			data: ByteArray
		): void {
			if (flag === _FLAG_DATA) {
				this.dispatchEvent(new CommandDone(
					data.readUTFBytes(data.length)
				));
			}
			else if (flag === _FLAG_NULL) {
				this.dispatchEvent(new CommandDone(null));
			}
			else {
				super._handleData(flag, data);
			}
		}
	}
}
