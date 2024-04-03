// For Ionic
import { Injectable } from '@angular/core';
import { Storage } from '@ionic/storage';

@Injectable()
export class KeyValueProvider {
	constructor(private storage: Storage) { }

	async get(key: string) {
		let payload = (await this.storage.get(key));
		return payload as T;
	}

	async remove(key: string) {
		await this.storage.remove(key);
		return true;
	}

	async set(key: string, val: any) {
		let payload = val;
		await this.storage.set(key, payload);
		return true;
	}

	async hasKey(key: string) {
		const allKeys = await this.storage.keys();
		const truthy = allKeys.some(k => k === key);
		return truthy;
	}
}

/* Use:
async getMyValue(){
const value = this.keyValueProvider.get('userData');
return value;
}
*/

