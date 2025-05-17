import { useInterval } from "./hooks";

export const KeepAlive = () => {
  useInterval(() => fetch("/keepalive", { method: "GET" }), 5 * 1000);
  return <></>;
};
