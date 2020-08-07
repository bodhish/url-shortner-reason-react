module Decode = {
  let code =
    Json.Decode.field("code", Json.Decode.optional(Json.Decode.string));
};

module UrlUtils = {
  /* Source: https://gist.github.com/dperini/729294 */
  let urlRegex =
    Js.Re.fromStringWithFlags(
      "^"
      ++ "(?:(?:(?:https?|ftp):)?\\/\\/)"
      ++ "(?:\\S+(?::\\S*)?@)?"
      ++ "(?:"
      ++ "(?!(?:10|127)(?:\\.\\d{1,3}){3})"
      ++ "(?!(?:169\\.254|192\\.168)(?:\\.\\d{1,3}){2})"
      ++ "(?!172\\.(?:1[6-9]|2\\d|3[0-1])(?:\\.\\d{1,3}){2})"
      ++ "(?:[1-9]\\d?|1\\d\\d|2[01]\\d|22[0-3])"
      ++ "(?:\\.(?:1?\\d{1,2}|2[0-4]\\d|25[0-5])){2}"
      ++ "(?:\\.(?:[1-9]\\d?|1\\d\\d|2[0-4]\\d|25[0-4]))"
      ++ "|"
      ++ "(?:"
      ++ "(?:"
      ++ "[a-z0-9\\u00a1-\\uffff]"
      ++ "[a-z0-9\\u00a1-\\uffff_-]{0,62}"
      ++ ")?"
      ++ "[a-z0-9\\u00a1-\\uffff]\\."
      ++ ")+"
      ++ "(?:[a-z\\u00a1-\\uffff]{2,}\\.?)"
      ++ ")"
      ++ "(?::\\d{2,5})?"
      ++ "(?:[/?#]\\S*)?"
      ++ "$",
      ~flags="i",
    );

  let isInvalid = (allowBlank, url) =>
    if (url |> String.trim |> String.length > 0) {
      !Js.Re.test_(urlRegex, url);
    } else {
      !allowBlank;
    };
};

let str = React.string;

type status =
  | Empty
  | Editing
  | Saving
  | Complete
  | Error;

type state = {
  url: string,
  status,
};

type action =
  | UpdateUrl(string)
  | SetCompleteStatus
  | SetErrorStatus
  | SetSavingStatus;

let handleCallback = (json, callbackCB, send) => {
  let code = json |> Decode.code;

  send(
    switch (code) {
    | Some(_) => SetCompleteStatus
    | None => SetErrorStatus
    },
  );
  callbackCB(code);
};

let post = (payload, callback, send) => {
  Js.Promise.(
    Fetch.fetchWithInit(
      Config.backendHost ++ "short_codes",
      Fetch.RequestInit.make(
        ~method_=Post,
        ~body=
          Fetch.BodyInit.make(Js.Json.stringify(Js.Json.object_(payload))),
        ~headers=Fetch.HeadersInit.make({"Content-Type": "application/json"}),
        (),
      ),
    )
    |> then_(Fetch.Response.json)
    |> then_(json => handleCallback(json, callback, send) |> resolve)
  )
  |> ignore;
};

let createShortCode = (url, send, callbackCB) => {
  let payload = Js.Dict.empty();
  Js.Dict.set(payload, "url", url |> Js.Json.string);
  send(SetSavingStatus);
  post(payload, callbackCB, send);
};

let reducer = (state, action) =>
  switch (action) {
  | SetCompleteStatus => {...state, status: Complete}
  | SetErrorStatus => {...state, status: Error}
  | SetSavingStatus => {...state, status: Saving}
  | UpdateUrl(url) => {status: Editing, url}
  };

let buttonClasses = url => {
  "text-center md:inline-flex items-center md:ml-2 py-3 px-4 mt-2 border border-gray-300 text-xs leading-4 font-medium rounded text-gray-700 bg-white  focus:outline-none focus:border-blue-300 focus:shadow-outline-blue active:text-gray-800 active:bg-gray-50 transition ease-in-out duration-150 "
  ++ (
    UrlUtils.isInvalid(false, url)
      ? "cursor-not-allowed bg-gray-200" : "hover:text-indigo-500 "
  );
};

let inputClasses = status => {
  "appearance-none block w-full bg-white border rounded py-3 px-4 mr-2 mt-2 leading-tight focus:outline-none focus:bg-white "
  ++ (
    switch (status) {
    | Empty => "border-gray-400 focus:border-gray-500"
    | Editing
    | Saving => "border-blue-400 focus:border-blue-500"
    | Complete => "border-green-400 focus:border-green-500"
    | Error => "border-red-400 focus:border-red-500"
    }
  );
};

[@react.component]
let make = (~callbackCB) => {
  let (state, send) = React.useReducer(reducer, {status: Empty, url: ""});
  <div className="w-full">
    <div className="w-full flex md:flex-row flex-col ">
      <div className="w-full">
        <label
          className="inline-block tracking-wide text-xs font-semibold hidden"
          htmlFor="url">
          {"Enter the url" |> str}
        </label>
        <input
          value={state.url}
          onChange={event =>
            send(UpdateUrl(ReactEvent.Form.target(event)##value))
          }
          className={inputClasses(state.status)}
          id="url"
          type_="url"
          placeholder="http://bodhish.in"
        />
      </div>
      <button
        disabled={UrlUtils.isInvalid(false, state.url)}
        onClick={_ => createShortCode(state.url, send, callbackCB)}
        className={buttonClasses(state.url)}>
        {React.string("Shorten")}
      </button>
    </div>
    {switch (state.status) {
     | Error =>
       <div className="text-red-500 p-1">
         {React.string("Requert failed Please enter a valid Url")}
       </div>
     | _ => React.null
     }}
  </div>;
};
