function doGet(e) {
  const calendar = CalendarApp.getDefaultCalendar();
  if (calendar == undefined) {
    Logger.log("No data");
    return ContentService.createTextOutput("no calendars found");
  }
  const now = new Date();
  const later = new Date(now.getTime() + 24 * 60 * 60 * 1000); // next 24h

  const events = calendar.getEvents(now, later);

  const result = events.map(ev => ({
    title: ev.getTitle(),
    start: ev.getStartTime(),
    end: ev.getEndTime()
  }));

  return ContentService
    .createTextOutput(JSON.stringify(result))
    .setMimeType(ContentService.MimeType.JSON);
}
