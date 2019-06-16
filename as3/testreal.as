package {
	import flash.display.MovieClip;
	import flash.text.TextFormat;
	import flash.text.TextField;
	import flash.events.ErrorEvent;
	import flash.utils.ByteArray;
	import profzoom.Config;
	import profzoom.commands.EnvGet;
	import profzoom.commands.FileRead;
	import profzoom.commands.FileWrite;
	import profzoom.events.CommandDone;
	import profzoom.events.CommandError;

	public class testreal extends MovieClip {
		public static const WIDTH: Number = 600;
		public static const HEIGHT: Number = 400;

		public var txtOutFmt: TextFormat = null;
		public var txtOut: TextField = null;
		public var errors: Boolean = false;

		public function testreal(): void {
			this.txtOutFmt = new TextFormat();
			this.txtOutFmt.font = 'Courier';

			this.txtOut = new TextField();
			this.txtOut.x = 2;
			this.txtOut.y = 2;
			this.txtOut.width = WIDTH - 4;
			this.txtOut.height = HEIGHT - 4;
			this.txtOut.border = true;
			this.txtOut.multiline = true;
			this.txtOut.wordWrap = true;
			this.addChild(this.txtOut);

			this.test();
		}

		public function test(): void {
			profzoom.Config.prefix = '/someprefix/';
			this.testEnvGet();
		}

		public function testEnvGet(): void {
			this.log('EnvGet:');
			var self: testreal = this;
			var itter: Function = function(env: String, expect: String): void {
				self.log('  TEST: ' + env);
				var cmd: EnvGet = new EnvGet();
				var onError: Function = function(err: CommandError): void {
					cmd.removeEventListener(CommandError.ERROR, onError);
					cmd.removeEventListener(CommandDone.TYPE, onDone);
					self.fail('CommandError', err.text);
					next();
				};
				var onDone: Function = function(e: CommandDone): void {
					cmd.removeEventListener(CommandError.ERROR, onError);
					cmd.removeEventListener(CommandDone.TYPE, onDone);
					self.assertEqual('value', e.data, expect);
					next();
				};
				cmd.addEventListener(CommandError.ERROR, onError);
				cmd.addEventListener(CommandDone.TYPE, onDone);
				cmd.read(env);
			};
			var queue: Array = [
				['IS_SET', 'SET'],
				['DOES_NOT_EXIST', null],
				['IS_EMPTY', '']
			];
			var next: Function = function() {
				if (queue.length) {
					itter.apply(this, queue.shift());
				}
				else {
					self.testFileRead();
				}
			};
			next();
		}

		public function testFileRead(): void {
			this.log('FileRead:');
			var self: testreal = this;
			var itter: Function = function(path: String, expect: String): void {
				self.log('  TEST: ' + path);
				var cmd: FileRead = new FileRead();
				var onError: Function = function(err: CommandError): void {
					cmd.removeEventListener(CommandError.ERROR, onError);
					cmd.removeEventListener(CommandDone.TYPE, onDone);
					if (expect === null) {
						self.pass('CommandError', err.text);
					}
					else {
						self.fail('CommandError', err.text);
					}
					next();
				};
				var onDone: Function = function(e: CommandDone): void {
					cmd.removeEventListener(CommandError.ERROR, onError);
					cmd.removeEventListener(CommandDone.TYPE, onDone);
					var d: ByteArray = e.data;
					var v: String = d ? d.readUTFBytes(d.length) : null;
					self.assertEqual('value', v, expect);
					next();
				};
				cmd.addEventListener(CommandError.ERROR, onError);
				cmd.addEventListener(CommandDone.TYPE, onDone);
				cmd.read(path);
			};
			var queue: Array = [
				['testreal-data/data.txt', 'readdata'],
				['testreal-data/doesnotexist.txt', null],
			];
			var next: Function = function() {
				if (queue.length) {
					itter.apply(this, queue.shift());
				}
				else {
					self.testFileWrite();
				}
			};
			next();
		}

		public function testFileWrite(): void {
			this.log('FileWrite:');
			var self: testreal = this;
			var itter: Function = function(
				path: String,
				data: *,
				createDir: Boolean,
				errors: Boolean
			): void {
				self.log('  TEST: ' + path);
				var cmd: FileWrite = new FileWrite();
				var onError: Function = function(err: CommandError): void {
					cmd.removeEventListener(CommandError.ERROR, onError);
					cmd.removeEventListener(CommandDone.TYPE, onDone);
					if (errors) {
						self.pass('CommandError', err.text);
					}
					else {
						self.fail('CommandError', err.text);
					}
					next();
				};
				var onDone: Function = function(e: CommandDone): void {
					cmd.removeEventListener(CommandError.ERROR, onError);
					cmd.removeEventListener(CommandDone.TYPE, onDone);
					self.assertEqual('value', e.data, null);
					next();
				};
				cmd.addEventListener(CommandError.ERROR, onError);
				cmd.addEventListener(CommandDone.TYPE, onDone);
				cmd.write(path, data, createDir);
			};
			var queue: Array = [
				[
					'testreal-data/write.txt',
					'write text',
					false,
					false
				],
				[
					'testreal-data/write.bin',
					this.bytesToData([
						0x00,
						0x11,
						0x22,
						0x33,
						0x44,
						0x55,
						0x66,
						0x77,
						0x88,
						0x99,
						0xAA,
						0xBB,
						0xCC,
						0xDD,
						0xEE,
						0xFF
					]),
					false,
					false
				],
				[
					'testreal-data/badsub/write.txt',
					'write text sub fail',
					false,
					true
				],
				[
					'testreal-data/wa/wb/file.txt',
					'write text sub pass',
					true,
					false
				]
			];
			var next: Function = function() {
				if (queue.length) {
					itter.apply(this, queue.shift());
				}
				else {
					self.testDone();
				}
			};
			next();
		}

		public function testDone(): void {
			this.log(this.errors ? 'FAIL' : 'PASS');
		}

		public function assertEqual(name: String, a: *, b: *): void {
			var pass: Boolean = a === b;
			if (!pass) {
				this.errors = true;
			}
			var aEnc: String = this.valueEncode(a);
			var bEnc: String = this.valueEncode(b);
			this.log(
				'    ' +
				(pass ? 'PASS' : 'FAIL') + ': ' +
				name + ': ' +
				aEnc + (pass ? ' === ' : ' !== ') + bEnc
			);
		}

		public function pass(name: String, msg: String) {
			this.log('    PASS: ' + name + ': ' + msg);
		}

		public function fail(name: String, msg: String) {
			this.log('    FAIL: ' + name + ': ' + msg);
		}

		public function valueEncode(val: *): String {
			if (typeof val === 'string') {
				return this.stringEncode(val);
			}
			return '' + val;
		}

		public function stringEncode(str: String): String {
			return '"' + str
				.split('\\').join('\\\\')
				.split('\0').join('\\0')
				.split('\n').join('\\n')
				.split('\r').join('\\r')
				.split('\t').join('\\t')
				.split('"').join('\\"') +
				'"';
		}

		public function stringLimit(str: String, num: uint): String {
			if (str.length > num) {
				return str.substr(0, num) + '...';
			}
			return str;
		}

		public function bytesToData(data: Array): ByteArray {
			var r: ByteArray = new ByteArray();
			for each (var b: * in data) {
				r.writeByte(b);
			}
			r.position = 0;
			return r;
		}

		public function log(str: String): void {
			this.txtOut.appendText(str);
			this.txtOut.appendText('\n');
			this.txtOut.setTextFormat(this.txtOutFmt);
		}
	}
}
