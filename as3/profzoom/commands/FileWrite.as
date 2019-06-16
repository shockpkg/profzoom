package profzoom.commands {
	import flash.utils.ByteArray;
	import profzoom.Command;
	import profzoom.events.CommandDone;

	public class FileWrite extends Command {
		/**
		 * Command hook name.
		 */
		protected static const _HOOK: String = 'filewrite';

		/**
		 * File writer.
		 */
		public function FileWrite() {
			super();
		}

		/**
		 * Write file.
		 *
		 * @param file File path.
		 * @param data File data, ByteArray, String, to cast to String.
		 * @param createDir Create directory if necessary.
		 */
		public function write(
			file: String,
			data: *,
			createDir: Boolean = false
		) {
			this._run([
				createDir ? '1' : '0',
				file,
				(data as ByteArray) ? data : ('' + data)
			]);
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
			if (flag === _FLAG_NULL) {
				this.dispatchEvent(new CommandDone(null));
			}
			else {
				super._handleData(flag, data);
			}
		}
	}
}
