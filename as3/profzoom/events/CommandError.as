package profzoom.events {
	import flash.events.ErrorEvent;

	public class CommandError extends ErrorEvent {
		public static const ERROR: String = 'profzoom.events.CommandError';

		public function CommandError(text: String = '', id: int = 0) {
			super(ERROR, false, false, text, id);
		}
	}
}
