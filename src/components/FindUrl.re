module Decode = {
  let url =
    Json.Decode.field("url", Json.Decode.optional(Json.Decode.string));
};

let redirect = path =>
  path |> Webapi.Dom.Window.setLocation(Webapi.Dom.window);

type state =
  | Loading
  | Loaded(url)
  | Failed
and url = string;

let getShortCode = (shortCode, callback, send, ()) => {
  Js.Promise.(
    Fetch.fetch(Config.backendHost ++ shortCode)
    |> then_(Fetch.Response.json)
    |> then_(json => callback(json, send) |> resolve)
  )
  |> ignore;

  None;
};

let callback = (json, setState) => {
  let url = Decode.url(json);
  setState(_ => url->Belt.Option.mapWithDefault(Failed, url => Loaded(url)));
  url->Belt.Option.mapWithDefault((), url => redirect(url) |> ignore);
};

let classes = state => {
  "text-center ml-2 py-3 px-4 text-md leading-4 font-medium rounded animate-pulse w-full font-bold "
  ++ (
    switch (state) {
    | Loading => "text-blue-700"
    | Loaded(_url) => "text-green-700"
    | Failed => "text-red-700"
    }
  );
};

[@react.component]
let make = (~code) => {
  let (state, setState) = React.useState(_ => Loading);
  React.useEffect1(getShortCode(code, callback, setState), [|code|]);
  <div className={classes(state)}>
    <div className="">
      {(
         switch (state) {
         | Loading => "Loading..."
         | Loaded(url) => "Redirecting to " ++ url ++ " ...."
         | Failed => "Invalid Short Code :" ++ code
         }
       )
       |> React.string}
    </div>
  </div>;
};
