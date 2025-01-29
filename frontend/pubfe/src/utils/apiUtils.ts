import axios from "axios";

export async function isApiReachable(ip: string): Promise<boolean> {
    try {
        await axios.get(`http://${ip}/api/state`);
        return true;
    } catch {
        return false;
    }
}