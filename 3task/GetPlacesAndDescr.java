import okhttp3.OkHttpClient;
import okhttp3.Request;
import okhttp3.Response;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.IOException;

public class GetPlacesAndDescr implements Runnable {
    private final LocationContext locationContext;

    private final static String APIKey = "5ae2e3f221c38a28845f05b62ccdfc52423ad30d150a59a0865ccb45";

    public GetPlacesAndDescr(LocationContext locationContext) {
        this.locationContext = locationContext;
    }


    public JSONArray getPlaces() throws IOException, JSONException {
        OkHttpClient client = new OkHttpClient();

        Request request = new Request.Builder()
                .url("https://api.opentripmap.com/0.1/en/places/radius?" +
                        "radius=500" +
                        "&lon=" + locationContext.getSelectedLocation().point().lng() +
                        "&lat=" + locationContext.getSelectedLocation().point().lat() +
                        "&apikey=" + APIKey
                )
                .get()
                .build();

        Response response = client.newCall(request).execute();

        return new JSONObject(response.body().string()).getJSONArray("features");
    }
    public void getDescriptions(JSONArray features) throws IOException, JSONException {

        System.out.println("");
        System.out.println("Interested places in this area:");
        System.out.println("_______________________________");
        for (int i = 0; i < features.length(); i++) {
            JSONObject jsonFeature = features.getJSONObject(i);

            String placeName = jsonFeature.getJSONObject("properties").getString("name");
            if (placeName.equals("")) continue;

            String description = jsonFeature.getJSONObject("properties").getString("kinds");
            System.out.println(placeName + " - " + description);

            }

    }
    public void makeRequest() throws IOException, JSONException {
        JSONArray features = getPlaces();
        getDescriptions(features);
    }

    @Override
    public void run() {
        try {
            makeRequest();
        } catch (IOException | JSONException e) {
            e.printStackTrace();
        }
    }
}
