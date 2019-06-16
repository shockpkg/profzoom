package profzoom.commands {
	import flash.utils.ByteArray;
	import profzoom.Command;
	import profzoom.events.CommandDone;

	public class FileRead extends Command {
		/**
		 * Command hook name.
		 */
		protected static const _HOOK: String = 'fileread';

		/**
		 * File reader.
		 */
		public function FileRead() {
			super();
		}

		/**
		 * Read file.
		 *
		 * @param file File path.
		 */
		public function read(file: String): String {
			this._run([file]);
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
				this.dispatchEvent(new CommandDone(data));
			}
			else {
				super._handleData(flag, data);
			}
		}
	}
}
