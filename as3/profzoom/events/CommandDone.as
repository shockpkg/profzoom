package profzoom.events {
	import flash.events.Event;

	public class CommandDone extends Event {
		public static const TYPE: String = 'profzoom.events.CommandDone';

		public var data: * = null;

		public function CommandDone(data: * = null) {
			super(TYPE, false, false);
			this.data = data;
		}
	}
}
