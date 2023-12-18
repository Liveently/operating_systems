import okhttp3.OkHttpClient;
import okhttp3.Request;
import okhttp3.Response;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.IOException;
import java.util.Scanner;

public class GetLocation implements Runnable {
    private final LocationContext locationContext;
    private static final String APIKey = "07813426-c567-4088-b25f-de54caff7e39";  //ключ API для доступа к сервису

    public GetLocation(LocationContext locationContext) {
        this.locationContext = locationContext;
    }

    public void makeRequest(String location) throws IOException, JSONException { //HTTP-запрос к API геокодирования
        OkHttpClient client = new OkHttpClient();

        Request request = new Request.Builder()
                .url("https://graphhopper.com/api/1/geocode?q=" + location + "&key=" + APIKey)
                .get()
                .build();

        Response response = client.newCall(request).execute();

        JSONArray jsonArray = new JSONObject(response.body().string()).getJSONArray("hits");


        for (int i = 0; i < jsonArray.length(); i++) {
            JSONObject jsonObject = jsonArray.getJSONObject(i);

            double lng = jsonObject.getJSONObject("point").getDouble("lng");
            double lat = jsonObject.getJSONObject("point").getDouble("lat");

            String name = jsonObject.getString("name");
            String country = jsonObject.getString("country");

            this.locationContext.getMap().put(i, new LocationInfo(name, new Point(lng, lat)));
            System.out.println(i + ") " + name + " " + country + " " + lng + " " + lat);
        }
    }

    @Override
    public void run() {
        Scanner scanner = new Scanner(System.in);
        System.out.print("Enter location: ");

        String location = scanner.nextLine();
        try {
            makeRequest(location);
        } catch (IOException | JSONException e) {
            e.printStackTrace();
        }
    }
}