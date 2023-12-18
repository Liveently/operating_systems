import okhttp3.OkHttpClient;
import okhttp3.Request;
import okhttp3.Response;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.IOException;
import java.util.Scanner;

public class GetWeather implements Runnable {
    private final LocationContext locationContext;

    private static final String APIKey = "f244ec564fe18b250f9952f161feaaea";

    public GetWeather(LocationContext locationContext) {
        this.locationContext = locationContext;
    }

    public void makeRequest() throws IOException, JSONException {
        OkHttpClient client = new OkHttpClient();

        Request request = new Request.Builder()
                .url("https://api.openweathermap.org/data/2.5/weather?" +
                        "lat=" + this.locationContext.getSelectedLocation().point().lat() +
                        "&lon=" + this.locationContext.getSelectedLocation().point().lng() +
                        "&units=metric" +
                        "&appid=" + APIKey)
                .get()
                .build();

        Response response = client.newCall(request).execute();


        JSONObject jsonObject = new JSONObject(response.body().string()).getJSONObject("main");

        double temp = jsonObject.getDouble("temp");
        double feelsLike = jsonObject.getDouble("feels_like");
        System.out.println("");
        System.out.println("Temperature in " + this.locationContext.getSelectedLocation().name() + ": " + temp + " °C, feels like " + feelsLike + " °C");
    }

    @Override
    public void run() {
        Scanner scanner = new Scanner(System.in);
        System.out.print("\nPlease choose in which one you interested: ");
        int idx = scanner.nextInt();
        LocationInfo location = this.locationContext.getMap().get(idx);
        this.locationContext.setSelectedLocation(location);
        try {
            makeRequest();
        } catch (IOException | JSONException e) {
            e.printStackTrace();
        }
    }
}