package profzoom {
	import flash.events.Event;
	import flash.events.EventDispatcher;
	import flash.events.IOErrorEvent;
	import flash.net.URLLoader;
	import flash.net.URLLoaderDataFormat;
	import flash.net.URLRequest;
	import flash.utils.ByteArray;
	import profzoom.Config;
	import profzoom.events.CommandDone;
	import profzoom.events.CommandError;

	[Event(name='CommandDone', type='profzoom.events.CommandDone')]

	[Event(name='CommandError', type='profzoom.events.CommandError')]

	public class Command extends EventDispatcher {
		/**
		 * Base64 charset.
		 */
		protected static const _B64_CHARS: Array =
			'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_-'
			.split('');

		/**
		 * Response flag type error.
		 */
		protected static const _FLAG_ERROR: uint = 0;

		/**
		 * Response flag type data.
		 */
		protected static const _FLAG_DATA: uint = 1;

		/**
		 * Response flag type null.
		 */
		protected static const _FLAG_NULL: uint = 2;

		/**
		 * Command hook name, subclasses should implement.
		 */
		protected static const _HOOK: String = null;

		/**
		 * URLLoader instance.
		 */
		protected var _urlLoader: URLLoader = null;

		/**
		 * Class must be subclasses.
		 */
		public function Command() {
			if (this.constructor === Command) {
				throw new Error('Command must be subclassed');
			}
		}

		/**
		 * Check if currently bending.
		 */
		public function get pending(): Boolean {
			return !!this._urlLoader;
		}

		/**
		 * Get the hook name.
		 */
		protected function get _hook(): String {
			return this.constructor._HOOK;
		}

		/**
		 * Begin the URL loader.
		 *
		 * @return URLLoader instance.
		 */
		protected function _urlLoaderBegin(): URLLoader {
			var u: URLLoader = new URLLoader();
			u.dataFormat = URLLoaderDataFormat.BINARY;
			u.addEventListener(IOErrorEvent.IO_ERROR, this._runError);
			u.addEventListener(Event.COMPLETE, this._runComplete);
			this._urlLoader = u;
			return u;
		}

		/**
		 * End the URL loader.
		 */
		protected function _urlLoaderEnd(): void {
			var u: URLLoader = this._urlLoader;
			u.removeEventListener(IOErrorEvent.IO_ERROR, this._runError);
			u.removeEventListener(Event.COMPLETE, this._runComplete);
			this._urlLoader = null;
		}

		/**
		 * Run command.
		 *
		 * @param args String or ByteArray instances.
		 */
		protected function _run(args: Array): void {
			if (this._urlLoader) {
				throw new CommandError('Command is currently pending');
			}

			// Encode the individual components.
			var pieces: Array = [
				Config.prefix + this._b64EncodeString(this._hook)
			];
			for each (var arg: * in args) {
				if (typeof arg === 'string') {
					pieces.push(arg.length ? this._b64EncodeString(arg) : '');
				}
				else {
					pieces.push(arg.length ? this._b64EncodeData(arg) : '');
				}
			}

			this._urlLoaderBegin().load(new URLRequest(pieces.join('$')));
		}

		/**
		 * Run command error callback.
		 *
		 * @param e Error event.
		 */
		protected function _runError(e: IOErrorEvent): void {
			this._urlLoaderEnd();

			// Failed to get response from native code for unknown reason.
			this.dispatchEvent(new CommandError(
				'Unknown error communicating with native code'
			));
		}

		/**
		 * Run command complete callback.
		 *
		 * @param e Complete event.
		 */
		protected function _runComplete(e: Event): void {
			this._urlLoaderEnd();

			// Get data, and make sure there is some.
			var data: ByteArray = e.target.data;
			var l: uint = data.length;
			if (!l) {
				this.dispatchEvent(new CommandError(
					'Native response is empty'
				));
				return;
			}

			// Pop off last byte, which is the flag.
			var t: uint = l - 1;
			var flag: uint = data[t];
			data.length = t;

			// If an error, handle it directly.
			if (flag === _FLAG_ERROR) {
				this.dispatchEvent(new CommandError(
					'Native' + (data.length ? ': ' + data.readUTFBytes(t) : '')
				));
				return;
			}

			// Otherwise handle with the data handler.
			this._handleData(flag, data);
		}

		/**
		 * Handle the response data.
		 * Should be overridden by a subclass.
		 * This implementation throws an error for unexpected flag.
		 *
		 * @param flag Flag value.
		 * @param data Byte data.
		 */
		protected function _handleData(flag: uint, data: ByteArray): void {
			// Default data handler, for unexpected flags.
			this.dispatchEvent(new CommandError('Unexpected flag: ' + flag));
		}

		/**
		 * Convert String to ByteArray.
		 *
		 * @param data String data.
		 */
		protected function _stringToData(data: String): ByteArray {
			var r: ByteArray = new ByteArray();
			r.writeUTFBytes(data);
			r.position = 0;
			return r;
		}

		/**
		 * Base64 encode String data.
		 *
		 * @param data String data.
		 */
		protected function _b64EncodeString(data: String): String {
			return this._b64EncodeData(this._stringToData(data));
		}

		/**
		 * Base64 encode ByteArray data.
		 *
		 * @param data ByteArray data.
		 */
		protected function _b64EncodeData(data: ByteArray): String {
			var chars: Array = _B64_CHARS;
			var r: Array = [];
			var l: uint = data.length;
			var o: uint = l % 3;
			var m: uint = l - o;
			var i: uint = 0;
			var a: uint;
			var b: uint;
			var c: uint;
			while (i < m) {
				a = data[i++];
				b = data[i++];
				c = data[i++];
				r.push(
					chars[a >> 2] +
					chars[((a & 3) << 4) | (b >> 4)] +
					chars[((b & 15) << 2) | (c >> 6)] +
					chars[c & 63]
				);
			}
			if (o === 1) {
				a = data[i];
				r.push(
					chars[a >> 2] +
					chars[(a & 3) << 4]
				);
			}
			else if (o === 2) {
				a = data[i];
				b = data[i + 1];
				r.push(
					chars[a >> 2] +
					chars[((a & 3) << 4) | (b >> 4)] +
					chars[(b & 15) << 2]
				);
			}
			return r.join('');
		}
	}
}
